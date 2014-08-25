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

class Bullet: public Component<Bullet> {
public:
};

struct GameCommandShot {
};

struct GameCommandMove {
	Vector2 Direction = Vector2::Zero;
};

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

	auto keyboard = gameHost.Keyboard();
	auto keyboardState = keyboard->State();
	
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
		
		float speed = 180.0f;
		
		littleWitch.Component<Transform2D>()->Position += (speed * direction * clock->FrameDuration().count());
	}
	{
		constexpr DurationSeconds castingTime = DurationSeconds{0.07};
		static DurationSeconds duration = castingTime;
		
		if (duration < castingTime) {
			duration += clock->FrameDuration();
		}
		
		if (keyboardState.IsKeyDown(Keys::Space))
		{
			auto assets = gameHost.AssetManager();
			
			if (duration >= castingTime)
			{
				auto bullet = gameWorld.CreateObject();
				auto texture = assets->Load<Texture2D>("FireBullet.png");
				bullet.AddComponent<SpriteRenderable>(texture);
				auto & transform = bullet.AddComponent<Transform2D>();
				transform.Scale = {0.8f, 0.8f};
				transform.Position = littleWitch.Component<Transform2D>()->Position;
				bullet.AddComponent<Bullet>();
				
				duration = DurationSeconds::zero();
			}
		}
	}
	{
		for (auto & entity: gameWorld.QueryComponents<Bullet, Transform2D>())
		{
			constexpr float speed = 600.0f;
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
