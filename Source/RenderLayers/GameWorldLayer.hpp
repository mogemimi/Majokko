//
//  Copyright (C) 2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef MAJOKKO_GAMEWORLDLAYER_E41D9D4C_6523_49BD_AB30_FC84E75C18E4_HPP
#define MAJOKKO_GAMEWORLDLAYER_E41D9D4C_6523_49BD_AB30_FC84E75C18E4_HPP

#include "Pomdog.Experimental/Compositing/RenderLayer.hpp"
#include <Pomdog.Experimental/Experimental.hpp>
#include <Pomdog/Pomdog.hpp>

namespace Majokko {

using namespace Pomdog;

struct PostProcessSettings {
    bool fxaaEnabled = false;
    bool grayscaleEnabled = false;
    bool sepiaToneEnabled = false;
    bool vignetteEnabled = false;
};


class GameWorldLayer: public RenderLayer {
public:
    GameWorldLayer(GameHost & gameHost, GameWorld & gameWorld);

    //void WindowSizeChanged(int width, int height);

    void Draw(GraphicsContext & graphicsContext, Renderer & renderer) override;

    void Camera(GameObject const& cameraObject);

    void FxaaEnabled(bool fxaaEnabled);

    bool FxaaEnabled() const;

private:
    void DrawScene(GraphicsContext & graphicsContext, Renderer & renderer,
        std::shared_ptr<RenderTarget2D> const& renderTarget,
        Transform2D const& transform, Camera2D const& camera);

private:
    GameWorld & gameWorld;
    FXAA fxaa;
    ScreenQuad screenQuad;
    GrayscaleEffect grayscaleEffect;
    SepiaToneEffect sepiaToneEffect;
    VignetteEffect vignetteEffect;

    std::array<std::shared_ptr<RenderTarget2D>, 2> renderTargets;
    GameObject cameraObject;
    Color color;
    PostProcessSettings postProcessSettings;
};

} // namespace Majokko

#endif // !defined(MAJOKKO_GAMEWORLDLAYER_E41D9D4C_6523_49BD_AB30_FC84E75C18E4_HPP)
