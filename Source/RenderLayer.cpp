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
	, fxaa(gameHost.GraphicsDevice())
	, color(Color::CornflowerBlue)
{
	auto graphicsDevice = gameHost.GraphicsDevice();
	auto window = gameHost.Window();

	renderTarget = std::make_shared<RenderTarget2D>(graphicsDevice,
		window->ClientBounds().Width, window->ClientBounds().Height);
	
	fxaa.SetViewport(window->ClientBounds().Width, window->ClientBounds().Height);

	blendState = BlendState::CreateNonPremultiplied(graphicsDevice);
}
//-----------------------------------------------------------------------
void GameWorldLayer::WindowSizeChanged(int width, int height)
{
}
//-----------------------------------------------------------------------
void GameWorldLayer::Camera(GameObject const& cameraObjectIn)
{
	POMDOG_ASSERT(cameraObjectIn);
	this->cameraObject = cameraObjectIn;
}
//-----------------------------------------------------------------------
void GameWorldLayer::Draw(GraphicsContext & graphicsContext, Renderer & renderer)
{
	constexpr bool enableFxaa = false;

	if (enableFxaa) {
		graphicsContext.SetRenderTarget(renderTarget);
	}
	
	graphicsContext.Clear(Color::CornflowerBlue);
	graphicsContext.SetBlendState(blendState);
	
	{
		POMDOG_ASSERT(cameraObject);
		auto camera = cameraObject.Component<Camera2D>();
		auto transform = cameraObject.Component<Transform2D>();
		
		POMDOG_ASSERT(transform && camera);
		DrawScene(graphicsContext, renderer, *transform, *camera);
	}

	if (enableFxaa) {
		graphicsContext.SetRenderTarget();
		graphicsContext.Clear(Color::CornflowerBlue);
		fxaa.SetTexture(renderTarget);
		fxaa.Draw(graphicsContext);
	}
}
//-----------------------------------------------------------------------
void GameWorldLayer::DrawScene(GraphicsContext & graphicsContext, Renderer & renderer,
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
