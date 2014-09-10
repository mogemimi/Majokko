//
//  Copyright (C) 2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef MAJOKKO_OBJECTFACTORY_32F71A3D_0A8A_44D6_9AE6_FDBC4C91E7C1_HPP
#define MAJOKKO_OBJECTFACTORY_32F71A3D_0A8A_44D6_9AE6_FDBC4C91E7C1_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "Pomdog.Experimental/Pomdog2D.hpp"
#include <Pomdog/Pomdog.hpp>

namespace Majokko {

using namespace Pomdog;

struct SkinnedMeshAnimatorResource {
	std::shared_ptr<Skeleton> Skeleton;
	std::shared_ptr<AnimationGraph> AnimationGraph;
	std::shared_ptr<SkinnedMesh> Mesh;
	std::shared_ptr<Texture2D> Texture;
};


class ObjectFactory {
public:
	GameObject CreateCamera(GameWorld & gameWorld);
	
	GameObject CreateLittleWitch(GameWorld & gameWorld, GraphicsDevice & graphicsDevice, AssetManager & assets);
	
	GameObject CreateGhost(GameWorld & gameWorld, GraphicsDevice & graphicsDevice, AssetManager & assets, Vector2 const& position);
	
private:
	Optional<SkinnedMeshAnimatorResource> littleWitchResource;
	Optional<SkinnedMeshAnimatorResource> ghostResource;
};

}// namespace Majokko

#endif // !defined(MAJOKKO_OBJECTFACTORY_32F71A3D_0A8A_44D6_9AE6_FDBC4C91E7C1_HPP)
