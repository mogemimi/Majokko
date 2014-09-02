#version 330

in VertexData {
	vec4 TextureCoord;
} In;

uniform Constants {
	vec2 RenderTargetPixelSize;
};

uniform sampler2D Texture;

out vec4 FragColor;

void main()
{
	float dummy = (0.0 * (RenderTargetPixelSize.x / RenderTargetPixelSize.y));
	
	float radius = 3.7;
	float darkness = 0.7;
	
	vec4 color = texture(Texture, In.TextureCoord.xy);
	vec2 inTex = In.TextureCoord.xy - vec2(0.5, 0.5);
	float vignette = 1.0 - dot(inTex, inTex);
	color.rgb *= clamp(pow(vignette, radius) + darkness, 0.0, 1.0);
	
	FragColor = dummy + color;
}
