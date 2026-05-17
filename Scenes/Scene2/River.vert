#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float tileS = 1.0f;
uniform float tileT = 1.0f;
uniform bool isTile = true;

uniform float tileS0;
uniform float tileT0;
uniform float tileS1;
uniform float tileT1;

out vec2 TexCoordTile0;
out vec2 TexCoordTile1;
out vec2 TexCoord;

uniform float uTime0;
uniform float uTime1;
uniform vec2 uScrollDirection = vec2(1.0, 0.0);

void main()
{
	if (isTile)
	{
		TexCoord = vec2(aTex.s * tileS, aTex.t * tileT);
		TexCoordTile0 = vec2(aTex.s * tileS0, aTex.t * tileT0);
		TexCoordTile1 = vec2(aTex.s * tileS1, aTex.t * tileT1);

		// Scroll the second texture coordinates over time
		TexCoordTile0 = TexCoordTile0 + uScrollDirection * uTime0;
		TexCoordTile1 = TexCoordTile1 + uScrollDirection * uTime1;

	}
	else
	{
		TexCoord = aTex;
		TexCoordTile0 = aTex;
		TexCoordTile1 = aTex;
	}

	gl_Position = projection * view * model * vec4(aPos, 1.0);
}