#version 330

in VertexData {
	vec4 TextureCoord;
} In;

uniform Constants {
	vec2 RenderTargetPixelSize;
};

uniform sampler2D Texture;

out vec4 FragColor;

///@param radius [0.0, 1.0], default: 0.5
///@param softness [0.0, 1.0], default: 0.05
float CircleVignette(vec2 textureCoord, float radius, float softness)
{
	// NOTE:
	// [0, 1] => [-0.5, +0.5]
	vec2 position = In.TextureCoord.xy - vec2(0.5);
	float len = length(position);
	float vignette = smoothstep(radius, radius - softness, len);
	return vignette;
}

///@param radius [0.0, 1.0], default: 0.5
///@param softness [0.0, 1.0], default: 0.05
///@param scale [0.0, 1.0] default: 1.0
float ScaledCircleVignette(vec2 textureCoord, float radius, float softness, vec2 scale)
{
	vec2 position = (In.TextureCoord.xy - vec2(0.5)) * scale;
	float len = length(position);
	float vignette = smoothstep(radius, radius - softness, len);
	return vignette;
}

void main()
{
	float dummy = (0.0 * (RenderTargetPixelSize.x / RenderTargetPixelSize.y));
	
	const float radius = 0.518;
	const float softness = 0.014;
	
	//float vignette = CircleVignette(In.TextureCoord.xy, radius, softness);
	
	float vignette = ScaledCircleVignette(In.TextureCoord.xy, radius, softness, vec2(1.0, 0.3))
		* ScaledCircleVignette(In.TextureCoord.xy, radius, softness, vec2(0.5, 0.9));
	
	vec4 color = texture(Texture, In.TextureCoord.xy);
	
	// Multiply blending
	//color.rgb *= vignette;
	
	// Linear Burn blending
	color.rgb = max(color.rgb + vignette - 1.0, 0.0);
	
	FragColor = dummy + color;
}
