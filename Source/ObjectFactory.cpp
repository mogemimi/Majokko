//
//  Copyright (C) 2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "ObjectFactory.hpp"
#include "Breakable.hpp"
#include "Pomdog.Experimental/Pomdog2D.hpp"

namespace Majokko {
namespace {
//-----------------------------------------------------------------------
static GameObject CreateLittleWicth_Impl(GameWorld & gameWorld,
	GraphicsDevice & graphicsDevice, AssetManager & assets)
{
	auto gameObject = gameWorld.CreateObject();

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
static GameObject CreateGhost_Impl(GameWorld & gameWorld,
	GraphicsDevice & graphicsDevice, AssetManager & assets)
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
GameObject ObjectFactory::CreateCamera(GameWorld & gameWorld)
{
	auto camera = gameWorld.CreateObject();
	camera.AddComponent<Camera2D>();
	camera.AddComponent<Transform2D>();
	return std::move(camera);
}
//-----------------------------------------------------------------------
GameObject ObjectFactory::CreateLittleWitch(GameWorld & gameWorld, GraphicsDevice & graphicsDevice, AssetManager & assets)
{
	auto littleWitch = CreateLittleWicth_Impl(gameWorld, graphicsDevice, assets);
	littleWitch.AddComponent<Transform2D>();
	littleWitch.AddComponent<Movable>();
	
	auto transform = littleWitch.Component<Transform2D>();
	transform->Scale = {0.6f, 0.6f};
	
	return std::move(littleWitch);
}
//-----------------------------------------------------------------------
GameObject ObjectFactory::CreateGhost(GameWorld & gameWorld, GraphicsDevice & graphicsDevice, AssetManager & assets, Vector2 const& position)
{
	auto ghost = CreateGhost_Impl(gameWorld, graphicsDevice, assets);
	auto transform = ghost.Component<Transform2D>();
	transform->Position = position;
	transform->Scale = {0.6f, 0.6f};

	auto & movable = ghost.AddComponent<Movable>();
	movable.Velocity = {-100.0f, 0.0f};

	auto & breakable = ghost.AddComponent<Breakable>();
	breakable.Health = 40.0f;

	auto & collider = ghost.AddComponent<Collider2D>();
	collider.Bounds.Radius = 120.0f;
	collider.Bounds.Center = Vector2::Zero;
	return std::move(ghost);
}
//-----------------------------------------------------------------------
}// namespace Majokko
