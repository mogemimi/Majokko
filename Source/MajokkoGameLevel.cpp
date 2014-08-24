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
MajokkoGameLevel::MajokkoGameLevel(GameHost & gameHost, GameWorld & world)
{
	auto graphicsDevice = gameHost.GraphicsDevice();
	auto assets = gameHost.AssetManager();
	{
		mainCamera = world.CreateObject();
		mainCamera.AddComponent<Camera2D>();
		mainCamera.AddComponent<Transform2D>();
	}
	{
		littleWitch = CreateLittleWicth(world, graphicsDevice, *assets);
		auto transform = littleWitch.Component<Transform2D>();
		transform->Scale = {0.6f, 0.6f};
	}
}
//-----------------------------------------------------------------------
void MajokkoGameLevel::Update(GameHost & gameHost, GameWorld & world)
{
	auto clock = gameHost.Clock();

	auto keyboard = gameHost.Keyboard();
	auto keyboardState = keyboard->State();
	
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
//-----------------------------------------------------------------------
}// namespace Majokko
