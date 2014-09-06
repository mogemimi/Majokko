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
	bool spaceKeyPressed = false;
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
		if (!spaceKeyPressed && keyboardState.IsKeyDown(Keys::Space))
		{
			eventQueue.Enqueue<GameCommandShot>();
			spaceKeyPressed = true;
		}
		
		if (spaceKeyPressed && keyboardState.IsKeyUp(Keys::Space))
		{
			spaceKeyPressed = false;
		}
	}
}

//-----------------------------------------------------------------------
static GameObject CreateLittleWicth(GameWorld & gameWorld,
	std::shared_ptr<GraphicsDevice> const& graphicsDevice, AssetManager & assets)
{
	auto gameObject = gameWorld.CreateObject();
	
	gameObject.AddComponent<Transform2D>();
	{
		auto skeletonDesc = assets.Load<Details::Spine::SkeletonDesc>("Majokko.Spine/Majokko.json");
		auto skeleton = std::make_shared<Skeleton>(Details::Spine::CreateSkeleton(skeletonDesc.Bones));

		auto skeletonTransform = std::make_shared<SkeletonTransform>();
		skeletonTransform->Pose = SkeletonPose::CreateBindPose(*skeleton);
		skeletonTransform->GlobalPose = SkeletonHelper::ToGlobalPose(*skeleton, skeletonTransform->Pose);
		{
			auto animationGraph = Details::Spine::LoadAnimationGraph(skeletonDesc, assets, "Majokko.Spine/AnimGraph.json");
			gameObject.AddComponent(std::make_unique<SkeletonAnimator>(skeleton, skeletonTransform, animationGraph));
		}
		{
			auto textureAtlas = assets.Load<Details::TexturePacker::TextureAtlas>("Majokko.Spine/Majokko.atlas");
			auto texture = assets.Load<Texture2D>("Majokko.Spine/Majokko.png");

			auto bindPose = SkeletonPose::CreateBindPose(*skeleton);
			auto mesh = std::make_shared<SkinnedMesh>(Details::Spine::CreateSkinnedMesh(graphicsDevice,
				SkeletonHelper::ToGlobalPose(*skeleton, bindPose),
				skeletonDesc, textureAtlas,
				Vector2(texture->Width(), texture->Height()), "default"));

			gameObject.AddComponent(std::make_unique<SkinnedMeshRenderable>(graphicsDevice, assets, skeleton, skeletonTransform, mesh, texture));
		}
	}
	return std::move(gameObject);
}
//-----------------------------------------------------------------------
static GameObject CreateGhost(GameWorld & gameWorld,
	std::shared_ptr<GraphicsDevice> const& graphicsDevice, AssetManager & assets)
{
	auto gameObject = gameWorld.CreateObject();
	
	gameObject.AddComponent<Transform2D>();
	{
		auto skeletonDesc = assets.Load<Details::Spine::SkeletonDesc>("Ghost.Spine/Ghost.json");
		auto skeleton = std::make_shared<Skeleton>(Details::Spine::CreateSkeleton(skeletonDesc.Bones));

		auto skeletonTransform = std::make_shared<SkeletonTransform>();
		skeletonTransform->Pose = SkeletonPose::CreateBindPose(*skeleton);
		skeletonTransform->GlobalPose = SkeletonHelper::ToGlobalPose(*skeleton, skeletonTransform->Pose);
		{
			auto animationGraph = Details::Spine::LoadAnimationGraph(skeletonDesc, assets, "Ghost.Spine/AnimGraph.json");
			gameObject.AddComponent(std::make_unique<SkeletonAnimator>(skeleton, skeletonTransform, animationGraph));
		}
		{
			auto textureAtlas = assets.Load<Details::TexturePacker::TextureAtlas>("Ghost.Spine/Ghost.atlas");
			auto texture = assets.Load<Texture2D>("Ghost.Spine/Ghost.png");

			auto bindPose = SkeletonPose::CreateBindPose(*skeleton);
			auto mesh = std::make_shared<SkinnedMesh>(Details::Spine::CreateSkinnedMesh(graphicsDevice,
				SkeletonHelper::ToGlobalPose(*skeleton, bindPose),
				skeletonDesc, textureAtlas,
				Vector2(texture->Width(), texture->Height()), "default"));

			gameObject.AddComponent(std::make_unique<SkinnedMeshRenderable>(graphicsDevice, assets, skeleton, skeletonTransform, mesh, texture));
		}
	}
	return std::move(gameObject);
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
		mainCamera = gameWorld.CreateObject();
		mainCamera.AddComponent<Camera2D>();
		mainCamera.AddComponent<Transform2D>();
	}
	{
		littleWitch = CreateLittleWicth(gameWorld, graphicsDevice, *assets);
		auto transform = littleWitch.Component<Transform2D>();
		transform->Scale = {0.6f, 0.6f};
		littleWitch.AddComponent<Movable>();
	}
	{
		auto background = gameWorld.CreateObject();
		background.AddComponent<Transform2D>();
		auto & rectangle = background.AddComponent<RectangleRenderable>();
		Color color1 = {84-40, 133-40, 153-40, 255};
		Color color2 = {190, 204, 207, 255};
		//Color color2 = {84, 133, 153, 255};
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
	{
		auto entity = gameWorld.CreateObject();
		entity.AddComponent<Transform2D>();
		auto particleClip = std::make_shared<ParticleClip>(Details::ParticleLoader::Load(*assets, "Particles/explosion_smoke.json"));
		auto texture = assets->Load<Texture2D>("Particles/explosion_smoke.png");
		auto blendState = BlendState::CreateNonPremultiplied(graphicsDevice);
		auto & particleSystem = entity.AddComponent<ParticleSystem>(particleClip);
		particleSystem.Loop(true);
		
		entity.AddComponent<ParticleRenderable>(texture, blendState);
	}
	{
		auto layer = std::make_shared<GameWorldLayer>(gameHost, gameWorld);
		layer->Camera(mainCamera);
		scene.AddLayer(layer);
	}
//	{
//		gameTimer.Scale(0.5f);
//		spawnTimer.Scale(0.5f);
//		castingTimer.Scale(0.5f);
//	}
}
//-----------------------------------------------------------------------
void MajokkoGameLevel::Update(GameHost & gameHost, GameWorld & gameWorld)
{
	UpdatePlayerInput(gameHost, gameWorld);
	{
		for (auto & entity: gameWorld.QueryComponents<Movable, Transform2D>())
		{
			auto transform = entity.Component<Transform2D>();
			auto movable = entity.Component<Movable>();
			transform->Position += (movable->Velocity * gameTimer.FrameDuration().count());
		}
	}
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
	}
	{
		for (auto & entity: gameWorld.QueryComponents<Breakable, Transform2D>())
		{
			auto transform = entity.Component<Transform2D>();
			if (transform->Position.X < -1000.0f) {
				entity.Destroy();
			}
		}
	}
	{
		constexpr DurationSeconds spawnInterval {0.7};
		if (spawnTimer.TotalTime() >= spawnInterval)
		{
			auto graphicsDevice = gameHost.GraphicsDevice();
			auto assets = gameHost.AssetManager();
			
			for (int i = 0; i < 3; ++i) {
				auto ghost = CreateGhost(gameWorld, graphicsDevice, *assets);
				auto transform = ghost.Component<Transform2D>();
				transform->Position = {600.0f, -100.0f + 100.0f * i};
				transform->Scale = {0.6f, 0.6f};
				
				auto & movable = ghost.AddComponent<Movable>();
				movable.Velocity = {-100.0f, 0.0f};
				
				auto & breakable = ghost.AddComponent<Breakable>();
				breakable.Health = 40.0f;
				
				auto & collider = ghost.AddComponent<Collider2D>();
				collider.Bounds.Radius = 120.0f;
				collider.Bounds.Center = Vector2::Zero;
			}
			spawnTimer.Reset();
		}
	}
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
					breakable->Damage(40.0f);
					bullet.Destroy();
					break;
				}
			}
		}
	}
	{
		for (auto & entity: gameWorld.QueryComponents<Breakable>())
		{
			auto breakable = entity.Component<Breakable>();
			breakable->ApplyDamage();
			
			if (breakable->IsDead()) {
				entity.Destroy();
			}
		}
	}
	{
		for (auto & entity: gameWorld.QueryComponents<ParticleSystem>())
		{
			auto paritcleSystem = entity.Component<ParticleSystem>();
			if (!paritcleSystem->IsAlive()) {
				entity.Destroy();
			}
		}
	}
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
			constexpr DurationSeconds castingTime = DurationSeconds{0.07};
			
			if (castingTimer.TotalTime() >= castingTime)
			{
				auto assets = gameHost.AssetManager();
			
				auto bullet = gameWorld.CreateObject();
				auto texture = assets->Load<Texture2D>("FireBullet.png");
				bullet.AddComponent<SpriteRenderable>(texture);
				auto & transform = bullet.AddComponent<Transform2D>();
				transform.Scale = {0.8f, 0.8f};
				transform.Position = littleWitch.Component<Transform2D>()->Position + Vector2{50.0f, 30.0f};
				
				bullet.AddComponent<Bullet>();
				auto & collider = bullet.AddComponent<Collider2D>();
				collider.Bounds.Radius = 20.0f;
				collider.Bounds.Center = Vector2::Zero;
				
				castingTimer.Reset();
			}
		}
	});

	auto keyboard = gameHost.Keyboard();
	auto & keyboardState = keyboard->State();
	
	static PlayerCommandTranslator translator;
	translator.Translate(gameTimer.FrameDuration(), keyboardState, eventQueue);

	eventQueue.Tick();
}
//-----------------------------------------------------------------------
}// namespace Majokko
