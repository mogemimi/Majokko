//
//  Copyright (C) 2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef MAJOKKO_SCENE_32DEB0F7_6567_4BF8_A063_FF976DA93121_HPP
#define MAJOKKO_SCENE_32DEB0F7_6567_4BF8_A063_FF976DA93121_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "RenderLayer.hpp"
#include "Pomdog.Experimental/Pomdog2D.hpp"
#include <Pomdog/Pomdog.hpp>

namespace Majokko {

using namespace Pomdog;

class Scene {
public:
	Scene(GameWindow const& window, GraphicsDevice & graphicsDevice);

	void AddLayer(std::shared_ptr<RenderLayer> const& layer);
	
	void RemoveLayer(std::shared_ptr<RenderLayer> const& layer);
	
	void Draw(GraphicsContext & graphicsContext, Renderer & renderer);
	
private:
	void SortLayers();
	
private:
	std::vector<std::shared_ptr<RenderLayer>> layers;
	bool dirtyFlag;
};

}// namespace Majokko

#endif // !defined(MAJOKKO_SCENE_32DEB0F7_6567_4BF8_A063_FF976DA93121_HPP)
