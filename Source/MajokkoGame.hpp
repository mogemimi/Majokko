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

#include "Pomdog2D.hpp"
#include <Pomdog/Pomdog.hpp>

namespace Majokko {

using namespace Pomdog;

class MajokkoGame: public Game {
public:
	explicit MajokkoGame(std::shared_ptr<GameHost> host);

	void Initialize();

	void Update();

	void Draw();

private:
	std::shared_ptr<GameHost> gameHost;
	std::shared_ptr<GraphicsContext> graphicsContext;
	std::shared_ptr<Texture2D> texture;
	std::shared_ptr<RenderTarget2D> renderTarget;
	std::unique_ptr<SpriteRenderer> spriteRenderer;
	std::unique_ptr<FXAA> fxaa;
};

}// namespace Majokko

#endif // !defined(MAJOKKOGAME_HPP)
