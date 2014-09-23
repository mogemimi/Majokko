//
//  Copyright (C) 2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "Scene.hpp"
#include <algorithm>

namespace Majokko {
//-----------------------------------------------------------------------
Scene::Scene(GameWindow const& window, GraphicsDevice & graphicsDevice)
	: dirtyFlag(true)
{
	auto const clientBounds = window.ClientBounds();
	
	renderTarget = std::make_shared<RenderTarget2D>(graphicsDevice,
		clientBounds.Width, clientBounds.Height, true,
		SurfaceFormat::R8G8B8A8_UNorm, DepthFormat::None);
}
//-----------------------------------------------------------------------
void Scene::AddLayer(std::shared_ptr<RenderLayer> const& layer)
{
	POMDOG_ASSERT(std::end(layers) == std::find(std::begin(layers), std::end(layers), layer));
	layers.push_back(layer);
	dirtyFlag = true;
}
//-----------------------------------------------------------------------
void Scene::RemoveLayer(std::shared_ptr<RenderLayer> const& layer)
{
	layers.erase(std::remove(std::begin(layers), std::end(layers), layer), std::end(layers));
}
//-----------------------------------------------------------------------
void Scene::SortLayers()
{
	std::sort(std::begin(layers), std::end(layers),
		[](std::shared_ptr<RenderLayer> const& a, std::shared_ptr<RenderLayer> const& b){
			return a->DrawOrder() < b->DrawOrder();
		});
}
//-----------------------------------------------------------------------
void Scene::Draw(GraphicsContext & graphicsContext, Renderer & renderer)
{
	if (dirtyFlag)
	{
		SortLayers();
		dirtyFlag = false;
	}

	graphicsContext.SetRenderTarget(renderTarget);
	graphicsContext.Clear(Color::CornflowerBlue);

	for (auto & layer: layers)
	{
		POMDOG_ASSERT(layer);
		layer->Draw(graphicsContext, renderer);
	}
	
	graphicsContext.SetRenderTarget();
}
//-----------------------------------------------------------------------
}// namespace Majokko
