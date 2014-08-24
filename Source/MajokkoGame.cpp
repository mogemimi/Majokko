//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "MajokkoGame.hpp"
#include "MajokkoGameLevel.hpp"
#include <utility>

namespace Majokko {
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
	
	level = std::make_unique<MajokkoGameLevel>(*gameHost, gameWorld);
	
	scene.AddLayer(std::make_shared<HUDLayer>());
	
	auto layer = std::make_shared<GameWorldLayer>(*gameHost, gameWorld);
	{
		auto mainCamera = gameWorld.CreateObject();
		mainCamera.AddComponent<Camera2D>();
		mainCamera.AddComponent<Transform2D>();
		layer->Camera(mainCamera);
	}
	scene.AddLayer(layer);
}
//-----------------------------------------------------------------------
void MajokkoGame::Update()
{
	auto clock = gameHost->Clock();

	for (auto & gameObject: gameWorld.QueryComponents<Animator>())
	{
		auto animator = gameObject.Component<Animator>();
		
		POMDOG_ASSERT(animator);
		animator->Update(*clock);
	}
	
	level->Update(*gameHost, gameWorld);
	gameWorld.Refresh();
}
//-----------------------------------------------------------------------
void MajokkoGame::Draw()
{
	for (auto & layer: scene)
	{
		layer->Draw(*graphicsContext, renderer);
	}

	graphicsContext->Present();
}
//-----------------------------------------------------------------------
}// namespace Majokko
