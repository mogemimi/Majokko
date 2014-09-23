//
//  Copyright (C) 2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef MAJOKKO_BREAKABLE_D831B327_2D52_4885_BEDB_CB71DA271251_HPP
#define MAJOKKO_BREAKABLE_D831B327_2D52_4885_BEDB_CB71DA271251_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../BoundingCircle.hpp"
#include <Pomdog/Pomdog.hpp>

namespace Majokko {

using namespace Pomdog;

class Movable: public Component<Movable> {
public:
	Vector2 Velocity = Vector2::Zero;
	Vector2 Thrust = Vector2::Zero;
};

class Bullet: public Component<Bullet> {
public:
};

class Breakable: public Component<Breakable> {
public:
	float Health;
	float TotalDamage = 0.0f;
	
	bool IsDead() const;
	
	void Damage(float damage);
	
	void ApplyDamage();
};

class Collider2D: public Component<Collider2D> {
public:
	BoundingCircle Bounds;
};

enum class EnemyState: std::uint8_t {
	Running,
	Dead,
};

class Enemy: public Component<Enemy> {
public:
	EnemyState State = EnemyState::Running;
};


//Miniboss
//Superboss
//FinalBoss

}// namespace Majokko

#endif // !defined(MAJOKKO_BREAKABLE_D831B327_2D52_4885_BEDB_CB71DA271251_HPP)
