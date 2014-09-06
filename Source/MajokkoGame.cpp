//
//  Copyright (C) 2014 mogemimi.
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
	, gameTimer(*gameHostIn->Clock())
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
	
	level = std::make_unique<MajokkoGameLevel>(*gameHost, gameTimer, gameWorld, scene);
	scene.AddLayer(std::make_shared<HUDLayer>());
}
//-----------------------------------------------------------------------
void MajokkoGame::Update()
{
	for (auto & gameObject: gameWorld.QueryComponents<Animator>())
	{
		auto animator = gameObject.Component<Animator>();
		POMDOG_ASSERT(animator);
		
		animator->Update(gameTimer.FrameDuration());
	}
	
	for (auto & gameObject: gameWorld.QueryComponents<ParticleSystem>())
	{
		auto particleSystem = gameObject.Component<ParticleSystem>();
		POMDOG_ASSERT(particleSystem);
		
		particleSystem->Simulate(gameObject, gameTimer.FrameDuration());
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
