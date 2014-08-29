//
//  Copyright (C) 2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "Breakable.hpp"

namespace Majokko {

bool Breakable::IsDead() const
{
	return this->Health <= 0.0f;
}

void Breakable::ApplyDamage()
{
	Health -= TotalDamage;
	TotalDamage = 0.0f;
}

void Breakable::Damage(float damage)
{
	POMDOG_ASSERT(damage >= 0.0f);
	TotalDamage += damage;
}

}// namespace Majokko
