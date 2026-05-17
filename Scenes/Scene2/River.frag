#version 330 core

in vec2 TexCoord;
in vec2 TexCoordTile0;
in vec2 TexCoordTile1;

out vec4 FragColor;

uniform sampler2D Texture0;
uniform sampler2D Texture1;

uniform vec4 objColor;

void main()
{
	vec4 Tex0 = texture(Texture0, TexCoordTile0);
	vec4 Tex1 = texture(Texture1, TexCoordTile1);

	FragColor = objColor * (Tex0 * Tex1);
}

