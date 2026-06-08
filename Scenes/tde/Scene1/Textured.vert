#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float tileS = 6.0f;
uniform float tileT = 6.0f;
uniform bool isTile = true;

out vec2 TexCoord;

void main()
{
	if (isTile)
		TexCoord = vec2(aTex.s * tileS, aTex.t * tileT);
	else
		TexCoord = aTex;

	gl_Position = projection * view * model * vec4(aPos, 1.0);
}
