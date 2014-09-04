//
//  Copyright (C) 2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "GrayscaleEffect.hpp"
#include "Pomdog/Graphics/detail/BuiltinShaderPool.hpp"
#include "Pomdog/Graphics/detail/ShaderBytecode.hpp"
#include "Pomdog/Graphics/ConstantBufferBinding.hpp"
#include "Pomdog/Graphics/CustomVertex.hpp"
#include "Pomdog/Graphics/EffectPass.hpp"
#include "Pomdog/Graphics/EffectParameter.hpp"
#include "Pomdog/Graphics/GraphicsContext.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/InputLayout.hpp"
#include "Pomdog/Graphics/PrimitiveTopology.hpp"
#include "Pomdog/Graphics/RenderTarget2D.hpp"
#include "Pomdog/Graphics/SamplerState.hpp"
#include "Pomdog/Graphics/VertexBuffer.hpp"
#include "Pomdog/Math/Vector3.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <array>

namespace Pomdog {
//-----------------------------------------------------------------------
GrayscaleEffect::GrayscaleEffect(std::shared_ptr<GraphicsDevice> const& graphicsDevice, AssetManager & assets)
{
	samplerLinear = SamplerState::CreateLinearWrap(graphicsDevice);
	
	{
		using ScreenQuadVertex = CustomVertex<Vector3, Vector2>;
		
		std::array<ScreenQuadVertex, 4> const verticesCombo = {
			Vector3{-1.0f, -1.0f, 0.5f}, Vector2{0.0f, 0.0f},
			Vector3{-1.0f,  1.0f, 0.5f}, Vector2{0.0f, 1.0f},
			Vector3{ 1.0f,  1.0f, 0.5f}, Vector2{1.0f, 1.0f},
			Vector3{ 1.0f, -1.0f, 0.5f}, Vector2{1.0f, 0.0f},
		};
		
		std::array<ScreenQuadVertex, 6> const vertices = {
			verticesCombo[0], verticesCombo[1], verticesCombo[2],
			verticesCombo[2], verticesCombo[3], verticesCombo[0],
		};
		
		vertexBuffer = std::make_shared<VertexBuffer>(graphicsDevice,
			vertices.data(), vertices.size(), sizeof(ScreenQuadVertex), BufferUsage::Immutable);
	}
	{
		effectPass = assets.Load<EffectPass>("Shaders/Grayscale");
		constantBuffers = std::make_shared<ConstantBufferBinding>(graphicsDevice, *effectPass);
		inputLayout = std::make_shared<InputLayout>(graphicsDevice, effectPass);
	}
}
//-----------------------------------------------------------------------
void GrayscaleEffect::SetViewport(float width, float height)
{
	Vector2 renderTargetSize(width, height);
	constantBuffers->Find("Constants")->SetValue(renderTargetSize);
}
//-----------------------------------------------------------------------
void GrayscaleEffect::SetTexture(std::shared_ptr<RenderTarget2D> const& textureIn)
{
	POMDOG_ASSERT(textureIn);
	texture = textureIn;
}
//-----------------------------------------------------------------------
void GrayscaleEffect::Draw(GraphicsContext & graphicsContext)
{
	POMDOG_ASSERT(texture);

	graphicsContext.SetSamplerState(0, samplerLinear);
	graphicsContext.SetTexture(0, texture);
	graphicsContext.SetInputLayout(inputLayout);
	graphicsContext.SetVertexBuffer(vertexBuffer);
	graphicsContext.SetEffectPass(effectPass);
	graphicsContext.SetConstantBuffers(constantBuffers);
	graphicsContext.Draw(PrimitiveTopology::TriangleList, vertexBuffer->VertexCount());
}
//-----------------------------------------------------------------------
}// namespace Pomdog
