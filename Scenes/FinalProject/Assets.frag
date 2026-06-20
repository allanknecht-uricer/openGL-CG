#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 WorldPos;

out vec4 FragColor;

uniform sampler2D uDiffuseMap;
uniform sampler2D uBrickMap;
uniform vec4 objColor;
uniform bool useMap;
uniform float uTime;

vec3 rgbPanel(vec3 wp)
{
	float t = wp.y * 0.025 + wp.x * 0.012 + uTime * 0.55;
	return 0.5 + 0.5 * cos(vec3(0.0, 2.0, 4.0) + t * 6.28318);
}

bool inCutawayRegion(vec3 wp)
{
	return wp.x > 33.0 && wp.x < 58.0 && wp.z > 5.0 && wp.z < 27.0;
}

void main()
{
	vec3 norm = normalize(Normal);
	vec3 albedo = objColor.rgb;

	if (useMap)
		albedo *= texture(uDiffuseMap, TexCoord).rgb;
	else if (inCutawayRegion(WorldPos) && WorldPos.y < 13.0)
		albedo *= texture(uBrickMap, TexCoord).rgb;
	else if (max(objColor.r, max(objColor.g, objColor.b)) < 0.005)
		albedo = rgbPanel(WorldPos);

	vec3 ambient = albedo * 0.42;

	vec3 dirDir = normalize(vec3(0.2, -1.0, 0.15));
	float dirDiff = max(dot(norm, -dirDir), 0.0);
	vec3 dirLight = albedo * dirDiff * 0.35;

	vec3 p1 = normalize(vec3(-70.0, 40.0, -70.0) - WorldPos);
	vec3 p2 = normalize(vec3(75.0, 30.0, 55.0) - WorldPos);
	vec3 p3 = normalize(vec3(0.0, 25.0, 85.0) - WorldPos);
	float d1 = max(dot(norm, p1), 0.0);
	float d2 = max(dot(norm, p2), 0.0);
	float d3 = max(dot(norm, p3), 0.0);

	vec3 pointLight = albedo * (d1 * vec3(0.15, 0.6, 0.9) * 0.45
		+ d2 * vec3(0.9, 0.1, 0.7) * 0.35
		+ d3 * vec3(0.2, 0.85, 0.35) * 0.3);

	vec3 emissive = vec3(0.0);
	if (!useMap && max(objColor.r, max(objColor.g, objColor.b)) < 0.005)
		emissive = albedo * 0.45;

	vec3 result = ambient + dirLight + pointLight + emissive;
	result = result / (result + vec3(1.0));
	FragColor = vec4(result, 1.0);
}
