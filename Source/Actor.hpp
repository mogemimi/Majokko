//
//  Copyright (C) 2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef MAJOKKO_ACTOR_D4E2E9F6_469D_41C9_B4F8_1A1A8229F533_HPP
#define MAJOKKO_ACTOR_D4E2E9F6_469D_41C9_B4F8_1A1A8229F533_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "Pomdog.Experimental/Pomdog2D.hpp"
#include <Pomdog/Pomdog.hpp>
#include <vector>
#include <utility>
#include <algorithm>

namespace Majokko {

using namespace Pomdog;

class Actor: public Component<Actor> {
public:
	void RunAction(std::unique_ptr<Action> && action)
	{
		POMDOG_ASSERT(action);
		actions.push_back(std::move(action));
	}

	void Act(GameObject & gameObject, AnimationTimeInterval const& frameDuration)
	{
		POMDOG_ASSERT(gameObject);
	
		if (actions.empty()) {
			return;
		}
	
		for (auto & action: actions) {
			POMDOG_ASSERT(action);
			action->Act(gameObject, frameDuration);
		}
		
		actions.erase(std::remove_if(std::begin(actions), std::end(actions),
			[](std::unique_ptr<Action> const& action) {
				POMDOG_ASSERT(action);
				return action->IsCompleted();
			}), std::end(actions));
	}
	
	void StopActions()
	{
		actions.clear();
	}
	
private:
	std::vector<std::unique_ptr<Action>> actions;
};

}// namespace Majokko

#endif // !defined(MAJOKKO_ACTOR_D4E2E9F6_469D_41C9_B4F8_1A1A8229F533_HPP)
