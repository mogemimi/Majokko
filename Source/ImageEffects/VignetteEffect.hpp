//
//  Copyright (C) 2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef MAJOKKO_VIGNETTEEFFECT_AAA2D308_9F5B_4046_AD6E_20E00AC52A9E_HPP
#define MAJOKKO_VIGNETTEEFFECT_AAA2D308_9F5B_4046_AD6E_20E00AC52A9E_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Content/AssetManager.hpp"
#include <memory>

namespace Pomdog {

class VignetteEffect {
public:
	VignetteEffect(std::shared_ptr<GraphicsDevice> const& graphicsDevice, AssetManager & assets);
	
	void SetViewport(float width, float height);
	void SetTexture(std::shared_ptr<RenderTarget2D> const& texture);
	
	void Apply(GraphicsContext & graphicsContext);
	
private:
	std::shared_ptr<RenderTarget2D> texture;
	std::shared_ptr<SamplerState> samplerLinear;
	std::shared_ptr<EffectPass> effectPass;
	std::shared_ptr<ConstantBufferBinding> constantBuffers;
	std::shared_ptr<InputLayout> inputLayout;
};

}// namespace Pomdog

#endif // !defined(MAJOKKO_VIGNETTEEFFECT_AAA2D308_9F5B_4046_AD6E_20E00AC52A9E_HPP)
