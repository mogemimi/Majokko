//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "MajokkoGame.hpp"
#include <utility>

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
MajokkoGame::MajokkoGame(std::shared_ptr<GameHost> const& gameHostIn)
	: gameHost(gameHostIn)
	, graphicsContext(gameHostIn->GraphicsContext())
	, renderer(gameHostIn->GraphicsContext(), gameHostIn->GraphicsDevice())
{
}
//-----------------------------------------------------------------------
void MajokkoGame::Initialize()
{
	auto window = gameHost->Window();
	window->Title("Majokko");
	window->AllowPlayerResizing(false);

	auto graphicsDevice = gameHost->GraphicsDevice();
	auto assets = gameHost->AssetManager();

	renderTarget = std::make_shared<RenderTarget2D>(graphicsDevice,
		window->ClientBounds().Width, window->ClientBounds().Height);
	texture = assets->Load<Texture2D>("Majokko.png");
	spriteRenderer = std::make_unique<SpriteRenderer>(graphicsContext, graphicsDevice);
	fxaa = std::make_unique<FXAA>(graphicsDevice);
	fxaa->SetViewport(window->ClientBounds().Width, window->ClientBounds().Height);

	auto blendState = BlendState::CreateNonPremultiplied(graphicsDevice);
	graphicsContext->SetBlendState(blendState);
	
	{
		mainCamera = gameWorld.CreateObject();
		mainCamera.AddComponent<Camera2D>();
		mainCamera.AddComponent<Transform2D>();
	}
	{
		auto player = gameWorld.CreateObject();
		player.AddComponent<Transform2D>();
		player.AddComponent<SpriteRenderable>(texture);
	}
}
//-----------------------------------------------------------------------
void MajokkoGame::Update()
{
}
//-----------------------------------------------------------------------
void MajokkoGame::Draw()
{
	constexpr bool enableFxaa = true;

	if (enableFxaa) {
		graphicsContext->SetRenderTarget(renderTarget);
	}
	
	graphicsContext->Clear(Color::CornflowerBlue);
	{
		auto camera = mainCamera.Component<Camera2D>();
		auto transform = mainCamera.Component<Transform2D>();
		
		POMDOG_ASSERT(transform && camera);
		DrawScene(*transform, *camera);
	}

	if (enableFxaa) {
		graphicsContext->SetRenderTarget();
		graphicsContext->Clear(Color::CornflowerBlue);
		fxaa->SetTexture(renderTarget);
		fxaa->Draw(*graphicsContext);
	}

	graphicsContext->Present();
}
//-----------------------------------------------------------------------
void MajokkoGame::DrawScene(Transform2D const& transform, Camera2D const& camera)
{
	auto clientBounds = gameHost->Window()->ClientBounds();
	
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

	graphicsContext->Viewport(viewport);
	renderer.Render(graphicsContext);
}
//-----------------------------------------------------------------------
}// namespace Majokko
