//
//  Copyright (C) 2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "MajokkoGameLevel.hpp"

namespace Majokko {
namespace {

class Movable: public Component<Movable> {
public:
	Vector2 Velocity = Vector2::Zero;
	Vector2 Thrust = Vector2::Zero;
};

class Bullet: public Component<Bullet> {
public:
};

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
GameObject CreateLittleWicth(GameWorld & gameWorld,
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

}// unnamed namespace
//-----------------------------------------------------------------------
MajokkoGameLevel::MajokkoGameLevel(GameHost & gameHost, GameWorld & gameWorld, Scene & scene)
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
		auto layer = std::make_shared<GameWorldLayer>(gameHost, gameWorld);
		layer->Camera(mainCamera);
		scene.AddLayer(layer);
	}
}
//-----------------------------------------------------------------------
void MajokkoGameLevel::Update(GameHost & gameHost, GameWorld & gameWorld)
{
	auto clock = gameHost.Clock();
	
	EventQueue eventQueue;
	
	constexpr DurationSeconds castingTime = DurationSeconds{0.07};
	static DurationSeconds duration = castingTime;
	
	if (duration < castingTime) {
		duration += clock->FrameDuration();
	}
	
	eventQueue.Connect([&](Event const& args)
	{
		if (auto event = args.As<GameCommandMove>()) {
			constexpr float MaxSpeed = 240.0f;
			constexpr float MaxThrust = 600.0f;
			
			auto normalizedDirection = Vector2::Normalize(event->Direction);
			auto movable = littleWitch.Component<Movable>();
			
			constexpr float mass = 1.0f;
			movable->Thrust = (MaxThrust * normalizedDirection);
			movable->Velocity += (mass * movable->Thrust * clock->FrameDuration().count());
			
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
			if (duration >= castingTime)
			{
				auto assets = gameHost.AssetManager();
			
				auto bullet = gameWorld.CreateObject();
				auto texture = assets->Load<Texture2D>("FireBullet.png");
				bullet.AddComponent<SpriteRenderable>(texture);
				auto & transform = bullet.AddComponent<Transform2D>();
				transform.Scale = {0.8f, 0.8f};
				transform.Position = littleWitch.Component<Transform2D>()->Position + Vector2{50.0f, 30.0f};
				bullet.AddComponent<Bullet>();
				
				duration = DurationSeconds::zero();
			}
		}
	});

	auto keyboard = gameHost.Keyboard();
	auto & keyboardState = keyboard->State();
	
	static PlayerCommandTranslator translator;
	translator.Translate(clock->FrameDuration(), keyboardState, eventQueue);

	eventQueue.Tick();

	{
		for (auto & entity: gameWorld.QueryComponents<Movable, Transform2D>())
		{
			auto transform = entity.Component<Transform2D>();
			auto movable = entity.Component<Movable>();
			transform->Position += (movable->Velocity * clock->FrameDuration().count());
		}
	}
	{
		for (auto & entity: gameWorld.QueryComponents<Bullet, Transform2D>())
		{
			constexpr float speed = 900.0f;
			Vector2 direction{1.0f, 0.0f};
			auto transform = entity.Component<Transform2D>();
			transform->Position += (direction * speed * clock->FrameDuration().count());

			if (transform->Position.X > 1000.0f) {
				entity.Destroy();
			}
		}
	}
}
//-----------------------------------------------------------------------
}// namespace Majokko
