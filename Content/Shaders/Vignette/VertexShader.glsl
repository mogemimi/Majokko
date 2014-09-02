#version 330

layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 TextureCoord;

out VertexData {
	vec4 TextureCoord;
} Out;

void main()
{
	gl_Position = vec4(Position.xyz, 1.0);
	Out.TextureCoord = vec4(TextureCoord.xy, 0.0, 0.0);
}
