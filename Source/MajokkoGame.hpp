//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef MAJOKKOGAME_HPP
#define MAJOKKOGAME_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "RenderLayer.hpp"
#include "Pomdog.Experimental/Pomdog2D.hpp"
#include <Pomdog/Pomdog.hpp>

namespace Majokko {

using namespace Pomdog;

class MajokkoGame: public Game {
public:
	explicit MajokkoGame(std::shared_ptr<GameHost> const& gameHost);

	void Initialize() override;

	void Update() override;

	void Draw() override;
		
private:
	std::shared_ptr<GameHost> gameHost;
	std::shared_ptr<GraphicsContext> graphicsContext;
	std::unique_ptr<GameLevel> level;
	Renderer renderer;
	GameWorld gameWorld;
	Scene scene;
};

}// namespace Majokko

#endif // !defined(MAJOKKOGAME_HPP)
