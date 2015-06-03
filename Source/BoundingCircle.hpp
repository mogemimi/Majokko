//
//  Copyright (C) 2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef MAJOKKO_BOUNDINGCIRCLE_0460B25A_8A4A_4894_A3EF_36AE2C2CEE7F_HPP
#define MAJOKKO_BOUNDINGCIRCLE_0460B25A_8A4A_4894_A3EF_36AE2C2CEE7F_HPP

#if (_MSC_VER > 1000)
#    pragma once
#endif

#include <Pomdog/Pomdog.hpp>

namespace Majokko {

using namespace Pomdog;

class BoundingCircle {
public:
    Vector2 Center;
    float Radius;

public:
    BoundingCircle() = default;

    ///@~Japanese
    /// @param center 球の中心点の位置
    /// @param radius 球の半径
    BoundingCircle(Vector2 const& center, float radius);

    ///@~Japanese
    /// @brief 指定された境界ボリュームを含むかどうかを確認します。
    ContainmentType Contains(BoundingCircle const&) const;

    ///@~Japanese
    /// @brief 指定された境界ボリュームと交差するかどうかを確認します。
    bool Intersects(BoundingCircle const&) const;
};

}// namespace Majokko

#endif // !defined(MAJOKKO_BOUNDINGCIRCLE_0460B25A_8A4A_4894_A3EF_36AE2C2CEE7F_HPP)
