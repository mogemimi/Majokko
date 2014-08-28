//
//  Copyright (C) 2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef MAJOKKO_MAJOKKOGAMELEVEL_5BBC9870_D2E1_4A31_B706_4DA15C161CFF_HPP
#define MAJOKKO_MAJOKKOGAMELEVEL_5BBC9870_D2E1_4A31_B706_4DA15C161CFF_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "RenderLayer.hpp"
#include "Pomdog.Experimental/Pomdog2D.hpp"
#include <Pomdog/Pomdog.hpp>

namespace Majokko {

using namespace Pomdog;

class MajokkoGameLevel: public GameLevel {
public:
	MajokkoGameLevel(GameHost & gameHost, Timer & gameTimer, GameWorld & world, Scene & scene);

	void Update(GameHost & gameHost, GameWorld & world) override;
	
private:
	GameObject mainCamera;
	GameObject littleWitch;
	Timer & gameTimer;
};

}// namespace Majokko

#endif // !defined(MAJOKKO_MAJOKKOGAMELEVEL_5BBC9870_D2E1_4A31_B706_4DA15C161CFF_HPP)
