//
//  Copyright (C) 2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "RenderLayer.hpp"

namespace Majokko {
namespace {

struct SandboxHelper {
	static Matrix3x2 CreateViewMatrix2D(Transform2D const& transform, Camera2D const& camera)
	{
		POMDOG_ASSERT(camera.Zoom > 0);
		return Matrix3x2::CreateTranslation({-transform.Position.X, -transform.Position.Y})*
			Matrix3x2::CreateRotation(-transform.Rotation) *
			Matrix3x2::CreateScale(camera.Zoom);
	}

	static Matrix4x4 CreateViewMatrix(Transform2D const& transform, Camera2D const& camera)
	{
		POMDOG_ASSERT(camera.Zoom > 0);
		return Matrix4x4::CreateTranslation({-transform.Position.X, -transform.Position.Y, 1.0f})*
			Matrix4x4::CreateRotationZ(-transform.Rotation) *
			Matrix4x4::CreateScale({camera.Zoom, camera.Zoom, 1});
	}
};

}// unnamed namespace

//-----------------------------------------------------------------------
std::int32_t RenderLayer::DrawOrder() const
{
	return drawOrder;
}
//-----------------------------------------------------------------------
void RenderLayer::DrawOrder(std::int32_t drawOrderIn)
{
	this->drawOrder = drawOrderIn;
}





//-----------------------------------------------------------------------
GameWorldLayer::GameWorldLayer(GameHost & gameHost, GameWorld & gameWorldIn)
	: gameWorld(gameWorldIn)
	, screenQuad(gameHost.GraphicsDevice())
	, fxaa(gameHost.GraphicsDevice())
	, grayscaleEffect(gameHost.GraphicsDevice(), *gameHost.AssetManager())
	, sepiaToneEffect(gameHost.GraphicsDevice(), *gameHost.AssetManager())
	, vignetteEffect(gameHost.GraphicsDevice(), *gameHost.AssetManager())
	, color(Color::CornflowerBlue)
	, postProcessSettings{false, false, false, true}
{
	auto graphicsDevice = gameHost.GraphicsDevice();
	auto window = gameHost.Window();

	for (auto & renderTarget: renderTargets)
	{
		renderTarget = std::make_shared<RenderTarget2D>(graphicsDevice,
			window->ClientBounds().Width, window->ClientBounds().Height,
			true, SurfaceFormat::R8G8B8A8_UNorm, DepthFormat::None);
	}
	
	fxaa.SetViewport(window->ClientBounds().Width, window->ClientBounds().Height);
	vignetteEffect.SetViewport(window->ClientBounds().Width, window->ClientBounds().Height);
	sepiaToneEffect.SetViewport(window->ClientBounds().Width, window->ClientBounds().Height);
	grayscaleEffect.SetViewport(window->ClientBounds().Width, window->ClientBounds().Height);

	blendStateNonPremultiplied = BlendState::CreateNonPremultiplied(graphicsDevice);
	blendStateAlphaBlend = BlendState::CreateAlphaBlend(graphicsDevice);
}
//-----------------------------------------------------------------------
//void GameWorldLayer::WindowSizeChanged(int width, int height)
//{
//}
//-----------------------------------------------------------------------
void GameWorldLayer::Camera(GameObject const& cameraObjectIn)
{
	POMDOG_ASSERT(cameraObjectIn);
	this->cameraObject = cameraObjectIn;
}
//-----------------------------------------------------------------------
void GameWorldLayer::FxaaEnabled(bool fxaaEnabledIn)
{
	this->postProcessSettings.fxaaEnabled = fxaaEnabledIn;
}
//-----------------------------------------------------------------------
bool GameWorldLayer::FxaaEnabled() const
{
	return this->postProcessSettings.fxaaEnabled;
}
//-----------------------------------------------------------------------
void GameWorldLayer::Draw(GraphicsContext & graphicsContext, Renderer & renderer)
{
	std::vector<std::function<void(std::shared_ptr<RenderTarget2D> const&)>> postProcessEffects;
	postProcessEffects.reserve(4);

	if (postProcessSettings.fxaaEnabled)
	{
		postProcessEffects.push_back([&](std::shared_ptr<RenderTarget2D> const& sourceTexture) {
			graphicsContext.Clear(Color::CornflowerBlue);
			fxaa.SetTexture(sourceTexture);
			fxaa.Apply(graphicsContext);
			screenQuad.DrawQuad(graphicsContext);
			graphicsContext.SetTexture(0);
		});
	}
	
	if (postProcessSettings.vignetteEnabled)
	{
		postProcessEffects.push_back([&](std::shared_ptr<RenderTarget2D> const& sourceTexture) {
			graphicsContext.Clear(Color::CornflowerBlue);
			vignetteEffect.SetTexture(sourceTexture);
			vignetteEffect.Apply(graphicsContext);
			screenQuad.DrawQuad(graphicsContext);
			graphicsContext.SetTexture(0);
		});
	}
		
	if (postProcessSettings.grayscaleEnabled)
	{
		postProcessEffects.push_back([&](std::shared_ptr<RenderTarget2D> const& sourceTexture) {
			graphicsContext.Clear(Color::CornflowerBlue);
			grayscaleEffect.SetTexture(sourceTexture);
			grayscaleEffect.Apply(graphicsContext);
			screenQuad.DrawQuad(graphicsContext);
			graphicsContext.SetTexture(0);
		});
	}
	
	if (postProcessSettings.sepiaToneEnabled)
	{
		postProcessEffects.push_back([&](std::shared_ptr<RenderTarget2D> const& sourceTexture) {
			graphicsContext.Clear(Color::CornflowerBlue);
			sepiaToneEffect.SetTexture(sourceTexture);
			sepiaToneEffect.Apply(graphicsContext);
			screenQuad.DrawQuad(graphicsContext);
			graphicsContext.SetTexture(0);
		});
	}

	auto outRenderTarget = renderTargets[0];
	auto inRenderTarget = renderTargets[1];

	if (!postProcessEffects.empty()) {
		graphicsContext.SetRenderTarget(outRenderTarget);
	}
	
	
	auto blendStateOld = graphicsContext.GetBlendState();

	{
		graphicsContext.Clear(Color::CornflowerBlue);
		graphicsContext.SetBlendState(blendStateNonPremultiplied);
	
		POMDOG_ASSERT(cameraObject);
		auto camera = cameraObject.Component<Camera2D>();
		auto transform = cameraObject.Component<Transform2D>();
		
		POMDOG_ASSERT(transform && camera);
		DrawScene(graphicsContext, renderer, outRenderTarget, *transform, *camera);
		
		std::swap(inRenderTarget, outRenderTarget);
	}
	
	graphicsContext.SetBlendState(blendStateAlphaBlend);
	
	size_t effectCount = 1;
	for (auto & applyPostEffect: postProcessEffects)
	{
		const bool effectEnd = effectCount >= postProcessEffects.size();
		if (effectEnd) {
			graphicsContext.SetRenderTarget();
		}
		else {
			graphicsContext.SetRenderTarget(outRenderTarget);
		}
		
		applyPostEffect(inRenderTarget);
		std::swap(inRenderTarget, outRenderTarget);
		++effectCount;
	}
	
	graphicsContext.SetBlendState(blendStateOld);
}
//-----------------------------------------------------------------------
void GameWorldLayer::DrawScene(GraphicsContext & graphicsContext, Renderer & renderer,
	std::shared_ptr<RenderTarget2D> const& renderTarget,
	Transform2D const& transform, Camera2D const& camera)
{
	auto clientBounds = renderTarget->Bounds();
	
	Viewport viewport(
		clientBounds.Width * camera.NormalizedViewportX,
		clientBounds.Height * camera.NormalizedViewportY,
		clientBounds.Width * camera.NormalizedViewportWidth,
		clientBounds.Height * camera.NormalizedViewportHeight);
	
	auto viewMatrix = SandboxHelper::CreateViewMatrix(transform, camera);
	auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
		viewport.Width(), viewport.Height(), camera.Near, camera.Far);

	renderer.ViewMatrix(viewMatrix);
	renderer.ProjectionMatrix(projectionMatrix);

	for (auto & gameObject: gameWorld.QueryComponents<Renderable, Transform2D>())
	{
		auto renderable = gameObject.Component<Renderable>();
		renderable->Visit(gameObject, renderer, viewMatrix, projectionMatrix);
	}

	graphicsContext.Viewport(viewport);
	renderer.Render(graphicsContext);
}




//-----------------------------------------------------------------------
void HUDLayer::Draw(GraphicsContext & graphicsContext, Renderer & renderer)
{
}






//-----------------------------------------------------------------------
void Scene::AddLayer(std::shared_ptr<RenderLayer> const& layer)
{
	POMDOG_ASSERT(std::end(layers) == std::find(std::begin(layers), std::end(layers), layer));
	layers.push_back(layer);
}
//-----------------------------------------------------------------------
void Scene::RemoveLayer(std::shared_ptr<RenderLayer> const& layer)
{
	layers.erase(std::remove(std::begin(layers), std::end(layers), layer), std::end(layers));
}
//-----------------------------------------------------------------------
void Scene::SortLayers()
{
	std::sort(std::begin(layers), std::end(layers),
		[](std::shared_ptr<RenderLayer> const& a, std::shared_ptr<RenderLayer> const& b){
			return a->DrawOrder() < b->DrawOrder();
		});
}



}// namespace Majokko
