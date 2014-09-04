//
//  Copyright (C) 2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef MAJOKKO_GRAYSCALEEFFECT_E498349A_BA58_486B_B534_54D83AEBE966_HPP
#define MAJOKKO_GRAYSCALEEFFECT_E498349A_BA58_486B_B534_54D83AEBE966_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <Pomdog/Pomdog.hpp>
#include <memory>

namespace Pomdog {

class GrayscaleEffect {
public:
	GrayscaleEffect(std::shared_ptr<GraphicsDevice> const& graphicsDevice, AssetManager & assets);
	
	void SetViewport(float width, float height);
	void SetTexture(std::shared_ptr<RenderTarget2D> const& texture);
	
	void Draw(GraphicsContext & graphicsContext);
	
private:
	std::shared_ptr<RenderTarget2D> texture;
	std::shared_ptr<SamplerState> samplerLinear;
	std::shared_ptr<VertexBuffer> vertexBuffer;
	std::shared_ptr<EffectPass> effectPass;
	std::shared_ptr<ConstantBufferBinding> constantBuffers;
	std::shared_ptr<InputLayout> inputLayout;
};

}// namespace Pomdog

#endif // !defined(MAJOKKO_GRAYSCALEEFFECT_E498349A_BA58_486B_B534_54D83AEBE966_HPP)
