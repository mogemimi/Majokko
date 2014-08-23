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
//-----------------------------------------------------------------------
MajokkoGame::MajokkoGame(std::shared_ptr<GameHost> const& gameHostIn)
	: gameHost(gameHostIn)
{
	graphicsContext = gameHost->GraphicsContext();
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
	
	spriteRenderer->Begin(SpriteSortMode::BackToFront);
	spriteRenderer->Draw(texture, Matrix3x2::Identity,
		Rectangle(0, 0, texture->Width(), texture->Height()), Color::White);
	spriteRenderer->End();

	if (enableFxaa) {
		graphicsContext->SetRenderTarget();
		graphicsContext->Clear(Color::CornflowerBlue);
		fxaa->SetTexture(renderTarget);
		fxaa->Draw(*graphicsContext);
	}

	graphicsContext->Present();
}
//-----------------------------------------------------------------------
}// namespace Majokko
