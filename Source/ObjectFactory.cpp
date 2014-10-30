//
//  Copyright (C) 2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "ObjectFactory.hpp"
#include "Breakable.hpp"
#include "Actor.hpp"

namespace Majokko {
namespace {
//-----------------------------------------------------------------------
static SkinnedMeshAnimatorResource CreateLittleWicth_Impl(
	GraphicsDevice & graphicsDevice, AssetManager & assets)
{
	SkinnedMeshAnimatorResource resource;
	
	auto skeletonDesc = assets.Load<Details::Spine::SkeletonDesc>("Majokko.Spine/Majokko.json");
	resource.Skeleton = std::make_shared<Skeleton>(Details::Spine::CreateSkeleton(skeletonDesc.Bones));
	resource.AnimationGraph = Details::Spine::LoadAnimationGraph(skeletonDesc, assets, "Majokko.Spine/AnimGraph.json");

	auto textureAtlas = assets.Load<Details::TexturePacker::TextureAtlas>("Majokko.Spine/Majokko.atlas");
	resource.Texture = assets.Load<Texture2D>("Majokko.Spine/Majokko.png");

	auto bindPose = SkeletonPose::CreateBindPose(*resource.Skeleton);
	resource.Mesh = std::make_shared<SkinnedMesh>(Details::Spine::CreateSkinnedMesh(graphicsDevice,
		SkeletonHelper::ToGlobalPose(*resource.Skeleton, bindPose),
		skeletonDesc, textureAtlas,
		Vector2(resource.Texture->Width(), resource.Texture->Height()), "default"));

	return std::move(resource);
}
//-----------------------------------------------------------------------
static SkinnedMeshAnimatorResource CreateGhost_Impl(
	GraphicsDevice & graphicsDevice, AssetManager & assets)
{
	SkinnedMeshAnimatorResource resource;
	
	auto skeletonDesc = assets.Load<Details::Spine::SkeletonDesc>("Ghost.Spine/Ghost.json");
	resource.Skeleton = std::make_shared<Skeleton>(Details::Spine::CreateSkeleton(skeletonDesc.Bones));
	resource.AnimationGraph = Details::Spine::LoadAnimationGraph(skeletonDesc, assets, "Ghost.Spine/AnimGraph.json");

	auto textureAtlas = assets.Load<Details::TexturePacker::TextureAtlas>("Ghost.Spine/Ghost.atlas");
	resource.Texture = assets.Load<Texture2D>("Ghost.Spine/Ghost.png");

	auto bindPose = SkeletonPose::CreateBindPose(*resource.Skeleton);
	resource.Mesh = std::make_shared<SkinnedMesh>(Details::Spine::CreateSkinnedMesh(graphicsDevice,
		SkeletonHelper::ToGlobalPose(*resource.Skeleton, bindPose),
		skeletonDesc, textureAtlas,
		Vector2(resource.Texture->Width(), resource.Texture->Height()), "default"));

	return std::move(resource);
}
//-----------------------------------------------------------------------
static void BuildObjectByAnimatorResource(GameObject & gameObject,
	SkinnedMeshAnimatorResource const& resource)
{
	auto skeletonTransform = std::make_shared<SkeletonTransform>();
	skeletonTransform->Pose = SkeletonPose::CreateBindPose(*resource.Skeleton);
	skeletonTransform->GlobalPose = SkeletonHelper::ToGlobalPose(*resource.Skeleton, skeletonTransform->Pose);
	gameObject.AddComponent(std::make_unique<Animator>(resource.Skeleton, skeletonTransform, resource.AnimationGraph));

	gameObject.AddComponent(std::make_unique<SkinnedMeshRenderable>(
		resource.Skeleton, skeletonTransform, resource.Mesh, resource.Texture));
}

}// unnamed namespace
//-----------------------------------------------------------------------
GameObject ObjectFactory::CreateCamera(GameWorld & gameWorld)
{
	auto entity = gameWorld.CreateObject();
	entity.AddComponent<Camera2D>();
	entity.AddComponent<Transform2D>();
	return std::move(entity);
}
//-----------------------------------------------------------------------
GameObject ObjectFactory::CreateLittleWitch(GameWorld & gameWorld, GraphicsDevice & graphicsDevice, AssetManager & assets)
{
	auto entity = gameWorld.CreateObject();
	
	auto & transform = entity.AddComponent<Transform2D>();
	transform.Scale = {0.6f, 0.6f};
	
	entity.AddComponent<Movable>();
	
	if (!littleWitchResource) {
		littleWitchResource = CreateLittleWicth_Impl(graphicsDevice, assets);
	}
	BuildObjectByAnimatorResource(entity, *littleWitchResource);

	return std::move(entity);
}
//-----------------------------------------------------------------------
GameObject ObjectFactory::CreateGhost(GameWorld & gameWorld, GraphicsDevice & graphicsDevice, AssetManager & assets, Vector2 const& position)
{
	auto entity = gameWorld.CreateObject();
	entity.AddComponent<Transform2D>();

	auto transform = entity.Component<Transform2D>();
	transform->Position = position;
	transform->Scale = {0.6f, 0.6f};

	auto & movable = entity.AddComponent<Movable>();
	movable.Velocity = {-100.0f, 0.0f};

	auto & breakable = entity.AddComponent<Breakable>();
	breakable.Health = 30.0f;

	auto & collider = entity.AddComponent<Collider2D>();
	collider.Bounds.Radius = 120.0f;
	collider.Bounds.Center = Vector2::Zero;
	
	if (!ghostResource) {
		ghostResource = CreateGhost_Impl(graphicsDevice, assets);
	}
	BuildObjectByAnimatorResource(entity, *ghostResource);
	
	entity.AddComponent<Actor>();
	entity.AddComponent<Enemy>();
	
	return std::move(entity);
}
//-----------------------------------------------------------------------
GameObject ObjectFactory::CreateBullet(GameWorld & gameWorld, AssetManager & assets, Vector2 const& position)
{
	auto bullet = gameWorld.CreateObject();
	auto texture = assets.Load<Texture2D>("FireBullet.png");
	auto & renderable = bullet.AddComponent<SpriteRenderable>(texture);
	renderable.DrawOrder = 500;
	
	auto & transform = bullet.AddComponent<Transform2D>();
	transform.Scale = {0.8f, 0.8f};
	transform.Position = position;

	bullet.AddComponent<Bullet>();
	auto & collider = bullet.AddComponent<Collider2D>();
	collider.Bounds.Radius = 20.0f;
	collider.Bounds.Center = Vector2::Zero;
	
	return std::move(bullet);
}
//-----------------------------------------------------------------------
}// namespace Majokko
