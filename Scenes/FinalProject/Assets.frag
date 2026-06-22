#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 WorldPos;
in vec3 VertexColor;

out vec4 FragColor;

uniform sampler2D uDiffuseMap;
uniform sampler2D uEmissiveMap;
uniform vec4 objColor;
uniform bool useMap;
uniform bool useEmissive;
uniform bool useVertexColor;
uniform bool uNgonPanel;
uniform bool uMetallic;
uniform float uTime;
uniform vec3 uViewPos;
uniform float uSpecScale;

#define MAX_NGON_LIGHTS 8
uniform int uNgonLightCount;
uniform vec3 uNgonLightPos[MAX_NGON_LIGHTS];
uniform vec3 uFillLightPos;
uniform float uFillLightStrength;

vec3 rgbPanel(vec3 wp)
{
	float t = wp.y * 0.025 + wp.x * 0.012 + uTime * 0.55;
	return 0.5 + 0.5 * cos(vec3(0.0, 2.0, 4.0) + t * 6.28318);
}

float specTerm(vec3 lightDir, vec3 norm, vec3 viewDir, float power)
{
	vec3 halfDir = normalize(lightDir + viewDir);
	return pow(max(dot(norm, halfDir), 0.0), power);
}

float ngonAttenuation(float dist)
{
	dist = max(dist, 8.0);
	return 1.0 / (1.0 + 0.012 * dist + 0.00018 * dist * dist);
}

float fillAttenuation(float dist)
{
	return 1.0 / (1.0 + 0.004 * dist + 0.00004 * dist * dist);
}

void main()
{
	vec3 norm = normalize(Normal);
	vec3 albedo = objColor.rgb;
	vec3 emissive = vec3(0.0);
    if (useVertexColor)
        albedo = VertexColor;
    else if (useMap)
        albedo *= texture(uDiffuseMap, TexCoord).rgb;
	if (useEmissive)
	{
		vec3 screenEmit = texture(uEmissiveMap, TexCoord).rgb;
		emissive += screenEmit * 1.4;
		albedo = max(albedo, screenEmit * 0.35);
	}
	else if (uNgonPanel)
	{
		albedo = rgbPanel(WorldPos);
		emissive = albedo * 1.15;
	}

	vec3 ambient = albedo * 0.32;

	vec3 pointLight = vec3(0.0);
	vec3 specular = vec3(0.0);
	vec3 viewDir = normalize(uViewPos - WorldPos);
	float specPower = 88.0;
	float specScale = 4.0 * uSpecScale;
	vec3 specTint = vec3(0.90, 0.92, 0.96);

	for (int i = 0; i < uNgonLightCount; ++i)
	{
		vec3 toLight = uNgonLightPos[i] - WorldPos;
		float dist = length(toLight);
		vec3 lightDir = toLight / dist;
		vec3 lightColor = rgbPanel(uNgonLightPos[i]);
		float att = ngonAttenuation(dist);
		float ndotl = dot(norm, lightDir);
		float diff = max(ndotl, 0.0);
		float soft = max(ndotl * 0.5 + 0.5, 0.0);
		float lit = max(diff, soft * 0.25);
		float wallGlow = exp(-(dist * dist) / 3600.0);

		pointLight += albedo * lit * lightColor * att * 2.2;
		pointLight += albedo * wallGlow * lightColor * att * 0.55;
		if (uMetallic)
			specular += lightColor * specTint * specTerm(lightDir, norm, viewDir, specPower) * specScale * att * 0.45;
	}

	vec3 fillDir = normalize(uFillLightPos - WorldPos);
	float fillDist = length(uFillLightPos - WorldPos);
	float fillDiff = max(dot(norm, fillDir), 0.0);
	vec3 fillLight = albedo * fillDiff * vec3(0.82, 0.85, 0.92) * uFillLightStrength * fillAttenuation(fillDist);

	vec3 result = ambient + pointLight + fillLight + specular + emissive;
	result = result / (result + vec3(1.0));
	FragColor = vec4(result, 1.0);
}
