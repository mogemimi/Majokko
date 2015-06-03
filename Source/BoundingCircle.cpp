//
//  Copyright (C) 2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "BoundingCircle.hpp"

namespace Majokko {
namespace Unused {
//-----------------------------------------------------------------------
BoundingCircle::BoundingCircle(Vector2 const& center, float radius)
    : Center(center)
    , Radius(radius)
{}
//-----------------------------------------------------------------------
ContainmentType BoundingCircle::Contains(BoundingCircle const& circle) const
{
    auto distance = Vector2::Distance(this->Center, circle.Center);
    if (distance > this->Radius + circle.Radius) {
        return ContainmentType::Disjoint;
    }
    if (distance + circle.Radius < this->Radius) {
        return ContainmentType::Contains;
    }
    return ContainmentType::Intersects;
}
//-----------------------------------------------------------------------
bool BoundingCircle::Intersects(BoundingCircle const& circle) const
{
    auto distance = Vector2::Distance(this->Center, circle.Center);
    return (distance <= this->Radius + circle.Radius);
}
//-----------------------------------------------------------------------
} // namespace Unused
} // namespace Majokko
