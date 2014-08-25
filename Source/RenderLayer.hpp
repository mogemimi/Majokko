//
//  Copyright (C) 2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef MAJOKKO_RENDERLAYER_8C6626CA_A7B4_4220_9CE7_80DE579A9E82_HPP
#define MAJOKKO_RENDERLAYER_8C6626CA_A7B4_4220_9CE7_80DE579A9E82_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "Pomdog.Experimental/Pomdog2D.hpp"
#include <Pomdog/Pomdog.hpp>

namespace Majokko {

using namespace Pomdog;

class RenderLayer {
public:
	virtual ~RenderLayer() = default;

	virtual void Draw(GraphicsContext & graphicsContext, Renderer & renderer) = 0;

	std::int32_t DrawOrder() const;
	void DrawOrder(std::int32_t drawOrder);
	
private:
	std::int32_t drawOrder = 0;
};




class GameWorldLayer: public RenderLayer {
public:
	GameWorldLayer(GameHost & gameHost, GameWorld & gameWorld);

	//void WindowSizeChanged(int width, int height);

	void Draw(GraphicsContext & graphicsContext, Renderer & renderer) override;
	
	void Camera(GameObject const& cameraObject);
	
	void FxaaEnabled(bool fxaaEnabled);
	
	bool FxaaEnabled() const;
	
private:
	void DrawScene(GraphicsContext & graphicsContext, Renderer & renderer,
		Transform2D const& transform, Camera2D const& camera);
	
private:
	GameWorld & gameWorld;
	FXAA fxaa;
	std::shared_ptr<RenderTarget2D> renderTarget;
	std::shared_ptr<BlendState> blendState;
	GameObject cameraObject;
	Color color;
	bool fxaaEnabled;
};




class HUDLayer: public RenderLayer {
public:
	void Draw(GraphicsContext & graphicsContext, Renderer & renderer) override;
	
private:
};




class Scene {
public:
	void AddLayer(std::shared_ptr<RenderLayer> const& layer);
	void RemoveLayer(std::shared_ptr<RenderLayer> const& layer);
	
	void SortLayers();
	
private:
	std::vector<std::shared_ptr<RenderLayer>> layers;
	
public:
	typedef decltype(layers)::iterator iterator;
	typedef decltype(layers)::const_iterator const_iterator;
	
	iterator begin() { return std::begin(layers); }
	iterator end() { return std::end(layers); };
	const_iterator begin() const { return std::begin(layers); }
	const_iterator end() const { return std::end(layers); };
};




}// namespace Majokko

#endif // !defined(MAJOKKO_RENDERLAYER_8C6626CA_A7B4_4220_9CE7_80DE579A9E82_HPP)
