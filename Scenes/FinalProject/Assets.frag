#version 330 core

in vec2 TexCoord;
in vec3 Normal;

out vec4 FragColor;

uniform sampler2D uDiffuseMap;
uniform vec4 objColor;
uniform bool useMap;

void main()
{
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(vec3(0.35, 1.0, 0.4));
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 lighting = vec3(0.28) + vec3(0.72) * diff;

	vec3 albedo = objColor.rgb;
	if (useMap)
		albedo *= texture(uDiffuseMap, TexCoord).rgb;

	FragColor = vec4(albedo * lighting, 1.0);
}
