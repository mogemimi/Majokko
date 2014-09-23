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

}// namespace Majokko

#endif // !defined(MAJOKKO_RENDERLAYER_8C6626CA_A7B4_4220_9CE7_80DE579A9E82_HPP)
