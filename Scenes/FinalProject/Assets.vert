#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in vec3 vertexColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoord;
out vec3 Normal;
out vec3 WorldPos;
out vec3 VertexColor;

void main()
{
	vec4 worldPos = model * vec4(position, 1.0);
	WorldPos = worldPos.xyz;
	TexCoord = vec2(texCoord.s, texCoord.t);
	VertexColor = vertexColor;
	Normal = mat3(transpose(inverse(model))) * normal;
	gl_Position = projection * view * worldPos;
}
