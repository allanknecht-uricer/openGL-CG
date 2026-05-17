#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoord0;
out vec2 TexCoord1;

uniform float uTime;
uniform vec2 uScrollDirection = vec2(1.0, 0.0);

void main()
{
	// Scroll the second texture coordinates over time
	TexCoord0 = aTex;
	TexCoord1 = aTex + uScrollDirection * uTime;

	gl_Position = projection * view * model * vec4(aPos, 1.0);
}