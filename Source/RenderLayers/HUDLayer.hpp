//
//  Copyright (C) 2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef MAJOKKO_HUDLAYER_EAB71C38_2316_4C5A_A5B2_1B75765C19CC_HPP
#define MAJOKKO_HUDLAYER_EAB71C38_2316_4C5A_A5B2_1B75765C19CC_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "Pomdog.Experimental/Compositing/RenderLayer.hpp"
#include <Pomdog/Pomdog.hpp>

namespace Majokko {

using namespace Pomdog;

class HUDLayer: public RenderLayer {
public:
	void Draw(GraphicsContext & graphicsContext, Renderer & renderer) override;
	
private:
};

}// namespace Majokko

#endif // !defined(MAJOKKO_HUDLAYER_EAB71C38_2316_4C5A_A5B2_1B75765C19CC_HPP)
