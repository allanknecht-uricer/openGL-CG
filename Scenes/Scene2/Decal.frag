#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D Texture0;
uniform sampler2D Texture1;

uniform vec4 objColor;

void main()
{
	vec4 Tex0 = texture(Texture0, TexCoord);
	vec4 Tex1 = texture(Texture1, TexCoord);

	FragColor = objColor * (Tex0 * Tex1);
}

