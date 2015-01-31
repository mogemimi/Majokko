//
//  Copyright (C) 2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "MajokkoGame.hpp"
#include "RenderLayers/HUDLayer.hpp"
#include "Actor.hpp"
#include "MajokkoGameLevel.hpp"
#include "Pomdog.Experimental/Rendering/Processors/ParticleBatchCommandProcessor.hpp"
#include "Pomdog.Experimental/Rendering/Processors/PrimitiveCommandProcessor.hpp"
#include "Pomdog.Experimental/Rendering/Processors/SkinnedMeshCommandProcessor.hpp"
#include "Pomdog.Experimental/Rendering/Processors/SpriteCommandProcessor.hpp"
#include <utility>

namespace Majokko {
//-----------------------------------------------------------------------
MajokkoGame::MajokkoGame(std::shared_ptr<GameHost> const& gameHostIn)
	: gameHost(gameHostIn)
	, graphicsContext(gameHostIn->GraphicsContext())
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
	
	level = std::make_unique<MajokkoGameLevel>(*gameHost, gameTimer, gameWorld, compositor);
	compositor.AddLayer(std::make_shared<HUDLayer>());
	
	{
		gameEditor = std::make_unique<SceneEditor::InGameEditor>(gameHost);

		auto stackPanel = std::make_shared<UI::StackPanel>(140, 170);
		stackPanel->Transform(Matrix3x2::CreateTranslation(Vector2{5, 10}));
		gameEditor->AddView(stackPanel);

		{
			auto navigator = std::make_shared<UI::DebugNavigator>(gameHost->Clock());
			stackPanel->AddChild(navigator);
		}
		{
			textBlock1 = std::make_shared<UI::TextBlock>();
			textBlock1->Text("Draw Calls: --");
			stackPanel->AddChild(textBlock1);
		}
	}
	{
		renderer.AddProcessor(typeid(Details::Rendering::ParticleBatchCommand),
			std::make_unique<ParticleBatchCommandProcessor>(graphicsContext, graphicsDevice));
		
		renderer.AddProcessor(typeid(Details::Rendering::PrimitiveCommand),
			std::make_unique<PrimitiveCommandProcessor>(graphicsContext, graphicsDevice));
		
		renderer.AddProcessor(typeid(Details::Rendering::SkinnedMeshCommand),
			std::make_unique<SkinnedMeshCommandProcessor>(graphicsDevice));
		
		renderer.AddProcessor(typeid(Details::Rendering::SpriteCommand),
			std::make_unique<SpriteCommandProcessor>(graphicsContext, graphicsDevice));
	}
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
	
	for (auto gameObject: gameWorld.QueryComponents<Actor>()) {
		auto actor = gameObject.Component<Actor>();
		actor->Act(gameObject, gameTimer.FrameDuration());
	}
	
	level->Update(*gameHost, gameWorld);
	gameWorld.Refresh();
	gameEditor->Update();
	textBlock1->Text(StringFormat("Draw Calls: %d", renderer.DrawCallCount()));
}
//-----------------------------------------------------------------------
void MajokkoGame::Draw()
{
	compositor.Draw(*graphicsContext, renderer);

	gameEditor->DrawGUI(*graphicsContext);
	graphicsContext->Present();
}
//-----------------------------------------------------------------------
}// namespace Majokko
