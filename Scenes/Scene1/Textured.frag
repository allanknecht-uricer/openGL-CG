#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D Texture0;
uniform vec4 objColor;

void main()
{
	vec4 color = vec4(1.0, 1.0, 1.0, 1.0);

	FragColor = objColor * texture(Texture0, TexCoord);
}

