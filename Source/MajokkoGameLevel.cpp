//
//  Copyright (C) 2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "MajokkoGameLevel.hpp"
#include "BoundingCircle.hpp"
#include "Breakable.hpp"

namespace Majokko {
namespace {

struct GameCommandShot {
};

struct GameCommandMove {
	Vector2 Direction = Vector2::Zero;
};

struct GameCommandStop {
};

class PlayerCommandTranslator {
public:
	void Translate(DurationSeconds const& frameDuration, KeyboardState const& keyboardState, EventQueue & eventQueue);
	
private:
	//bool spaceKeyPressed = false;
};

void PlayerCommandTranslator::Translate(DurationSeconds const& frameDuration, KeyboardState const& keyboardState, EventQueue & eventQueue)
{
	{
		Vector2 direction = Vector2::Zero;

		if (keyboardState.IsKeyDown(Keys::W)) {
			direction.Y += 1.0f;
		}
		if (keyboardState.IsKeyDown(Keys::A)) {
			direction.X += -1.0f;
		}
		if (keyboardState.IsKeyDown(Keys::S)) {
			direction.Y += -1.0f;
		}
		if (keyboardState.IsKeyDown(Keys::D)) {
			direction.X += 1.0f;
		}
		
		if (direction.LengthSquared() > std::numeric_limits<float>::epsilon()) {
			eventQueue.Enqueue<GameCommandMove>(direction);
		}
		else {
			eventQueue.Enqueue<GameCommandStop>();
		}
	}
	{
		if (keyboardState.IsKeyDown(Keys::Space))
		{
			eventQueue.Enqueue<GameCommandShot>();
		}
	}
//	{
//		if (!spaceKeyPressed && keyboardState.IsKeyDown(Keys::Space))
//		{
//			eventQueue.Enqueue<GameCommandShot>();
//			spaceKeyPressed = true;
//		}
//		
//		if (spaceKeyPressed && keyboardState.IsKeyUp(Keys::Space))
//		{
//			spaceKeyPressed = false;
//		}
//	}
}

}// unnamed namespace
//-----------------------------------------------------------------------
MajokkoGameLevel::MajokkoGameLevel(GameHost & gameHost, Timer & gameTimerIn, GameWorld & gameWorld, Scene & scene)
	: gameTimer(gameTimerIn)
	, spawnTimer(*gameHost.Clock())
	, castingTimer(*gameHost.Clock())
{
	auto graphicsDevice = gameHost.GraphicsDevice();
	auto assets = gameHost.AssetManager();
	{
		mainCamera = factory.CreateCamera(gameWorld);
		littleWitch = factory.CreateLittleWitch(gameWorld, *graphicsDevice, *assets);
	}
	{
		auto background = gameWorld.CreateObject();
		background.AddComponent<Transform2D>();
		auto & rectangle = background.AddComponent<RectangleRenderable>();
		Color color1 = {84-40, 133-40, 153-40, 255};
		Color color2 = {190, 204, 207, 255};
		rectangle.LeftTopColor(color1);
		rectangle.RightTopColor(color1);
		rectangle.LeftBottomColor(color2);
		rectangle.RightBottomColor(color2);
		auto bounds = gameHost.Window()->ClientBounds();
		rectangle.BoundingBox({0, 0, bounds.Width, bounds.Height});
		rectangle.DrawOrder = -1000.0f;
		
		///@todo badcode
		auto depthStencilState = DepthStencilState::CreateNone(graphicsDevice);
		gameHost.GraphicsContext()->SetDepthStencilState(depthStencilState);
	}
//	{
//		auto entity = gameWorld.CreateObject();
//		entity.AddComponent<Transform2D>();
//		auto particleClip = std::make_shared<ParticleClip>(Details::ParticleLoader::Load(*assets, "Particles/explosion_smoke.json"));
//		auto texture = assets->Load<Texture2D>("Particles/explosion_smoke.png");
//		auto blendState = BlendState::CreateNonPremultiplied(graphicsDevice);
//		auto & particleSystem = entity.AddComponent<ParticleSystem>(particleClip);
//		particleSystem.Loop(true);
//		
//		entity.AddComponent<ParticleRenderable>(texture, blendState);
//	}
	{
		auto layer = std::make_shared<GameWorldLayer>(gameHost, gameWorld);
		layer->Camera(mainCamera);
		scene.AddLayer(layer);
	}
//	{
//		constexpr float timeScale = 0.3f;
//		gameTimer.Scale(timeScale);
//		spawnTimer.Scale(timeScale);
//		castingTimer.Scale(timeScale);
//	}
}
//-----------------------------------------------------------------------
void MajokkoGameLevel::Update(GameHost & gameHost, GameWorld & gameWorld)
{
	auto UpdateMovable = [&]
	{
		for (auto & entity: gameWorld.QueryComponents<Movable, Transform2D>())
		{
			auto transform = entity.Component<Transform2D>();
			auto movable = entity.Component<Movable>();
			transform->Position += (movable->Velocity * gameTimer.FrameDuration().count());
		}
	};
	
	auto UpdateBullet = [&]
	{
		for (auto & entity: gameWorld.QueryComponents<Bullet, Transform2D>())
		{
			constexpr float speed = 900.0f;
			Vector2 direction{1.0f, 0.0f};
			auto transform = entity.Component<Transform2D>();
			transform->Position += (direction * speed * gameTimer.FrameDuration().count());

			if (transform->Position.X > 1000.0f) {
				entity.Destroy();
			}
		}
	};
	
	auto DestroyUnusedBreakable = [&]
	{
		for (auto & entity: gameWorld.QueryComponents<Breakable, Transform2D>())
		{
			auto transform = entity.Component<Transform2D>();
			if (transform->Position.X < -1000.0f) {
				entity.Destroy();
			}
		}
	};
	
	auto SpawnGhost = [&]
	{
		constexpr DurationSeconds spawnInterval {0.7};
		if (spawnTimer.TotalTime() >= spawnInterval)
		{
			auto graphicsDevice = gameHost.GraphicsDevice();
			auto assets = gameHost.AssetManager();
			
			for (int i = 0; i < 3; ++i) {
				Vector2 position = {600.0f, -100.0f + 100.0f * i};
				factory.CreateGhost(gameWorld, *graphicsDevice, *assets, position);
			}
			spawnTimer.Reset();
		}
	};
	
	auto CheckCollision = [&]
	{
		auto breakables = gameWorld.QueryComponents<Breakable, Collider2D>();
	
		for (auto & bullet: gameWorld.QueryComponents<Bullet, Collider2D>())
		{
			if (!bullet) {
				continue;
			}
		
			for (auto & enemy: breakables)
			{
				if (!enemy) {
					continue;
				}
			
				POMDOG_ASSERT(bullet);
			
				auto entityCollider = enemy.Component<Collider2D>();
				auto entityTransform = enemy.Component<Transform2D>();
				
				auto bulletCollider = bullet.Component<Collider2D>();
				auto bulletTransform = bullet.Component<Transform2D>();
				
				auto entityBounds = entityCollider->Bounds;
				auto bulletBounds = bulletCollider->Bounds;
				
				entityBounds.Center += entityTransform->Position;
				entityBounds.Radius *= std::max(entityTransform->Scale.X, entityTransform->Scale.Y);
				
				bulletBounds.Center += bulletTransform->Position;
				bulletBounds.Radius *= std::max(bulletTransform->Scale.X, bulletTransform->Scale.Y);
				
				if (ContainmentType::Contains == entityBounds.Contains(bulletBounds))
				{
					auto breakable = enemy.Component<Breakable>();
					breakable->Damage(10.0f);
					bullet.Destroy();
					break;
				}
			}
		}
	};
	
	auto ApplyDamageToBreakable = [&]
	{
		for (auto & entity: gameWorld.QueryComponents<Breakable>())
		{
			auto breakable = entity.Component<Breakable>();
			breakable->ApplyDamage();
			
			if (breakable->IsDead()) {
				entity.Destroy();
			}
		}
	};
	
	auto DestroyUnusedParticles = [&]
	{
		for (auto & entity: gameWorld.QueryComponents<ParticleSystem>())
		{
			auto paritcleSystem = entity.Component<ParticleSystem>();
			if (!paritcleSystem->IsAlive()) {
				entity.Destroy();
			}
		}
	};
	
	UpdatePlayerInput(gameHost, gameWorld);
	UpdateMovable();
	UpdateBullet();
	DestroyUnusedBreakable();
	CheckCollision();
	ApplyDamageToBreakable();
	SpawnGhost();
	DestroyUnusedParticles();
}
//-----------------------------------------------------------------------
void MajokkoGameLevel::UpdatePlayerInput(GameHost & gameHost, GameWorld & gameWorld)
{
	EventQueue eventQueue;
	eventQueue.Connect([&](Event const& args)
	{
		if (auto event = args.As<GameCommandMove>()) {
			constexpr float MaxSpeed = 300.0f;
			constexpr float MaxThrust = 1000.0f;
			
			auto normalizedDirection = Vector2::Normalize(event->Direction);
			auto movable = littleWitch.Component<Movable>();
			
			constexpr float mass = 1.0f;
			movable->Thrust = (MaxThrust * normalizedDirection);
			movable->Velocity += (mass * movable->Thrust * gameTimer.FrameDuration().count());
			
			if (movable->Velocity.LengthSquared() > MaxSpeed * MaxSpeed) {
				movable->Thrust = Vector2::Zero;
				movable->Velocity = Vector2::Normalize(movable->Velocity) * MaxSpeed;
			}
		}
		else if (args.Is<GameCommandStop>()) {
			auto movable = littleWitch.Component<Movable>();
			
			movable->Thrust = Vector2::Zero;
			movable->Velocity = Vector2::Zero;
		}
		else if (args.Is<GameCommandShot>()) {
			constexpr DurationSeconds castingTime = std::chrono::milliseconds(55);
			
			if (castingTimer.TotalTime() >= castingTime)
			{
				auto assets = gameHost.AssetManager();
				auto position = littleWitch.Component<Transform2D>()->Position + Vector2{50.0f, 30.0f};
				auto bullet = factory.CreateBullet(gameWorld, *assets, position);
				
				castingTimer.Reset();
			}
		}
	});
	
	{
		auto movable = littleWitch.Component<Movable>();
		auto anim = littleWitch.Component<Animator>();
		
		if (movable->Velocity.LengthSquared() == 0.0f) {
			if (anim->GetCurrentStateName() != "Idle") {
				anim->CrossFade("Idle", std::chrono::milliseconds(400));
			}
		}
		else {
			if (anim->GetCurrentStateName() != "Run") {
				anim->CrossFade("Run", std::chrono::milliseconds(300));
			}
		
			constexpr float MaxSpeed = 300.0f;
			const auto min = -MaxSpeed;
			const auto max = MaxSpeed;
			const auto range = max - min;
			const auto normalizedWeight = (movable->Velocity.X - min) / range;
			
			anim->SetFloat("Weight", normalizedWeight);
		}
	}

	auto keyboard = gameHost.Keyboard();
	auto & keyboardState = keyboard->State();
	
	static PlayerCommandTranslator translator;
	translator.Translate(gameTimer.FrameDuration(), keyboardState, eventQueue);

	eventQueue.Tick();
}
//-----------------------------------------------------------------------
}// namespace Majokko
