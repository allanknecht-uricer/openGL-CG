#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D Texture0;
uniform vec4 objColor;

void main()
{
	FragColor = objColor * texture(Texture0, TexCoord);
}
