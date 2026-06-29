#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 WorldPos;
in vec3 VertexColor;

out vec4 FragColor;

// Material (setado por mesh no CLoadAssets::Draw)
uniform sampler2D uDiffuseMap;   // textura de cor (chao, paredes, sofa, laptop...)
uniform sampler2D uEmissiveMap;    // textura emissive (tela acesa do laptop, ngons nao usam)
uniform vec4 objColor;             // cor base do material (multiplica textura ou cor sozinha)
uniform bool useMap;               // 1 = usa uDiffuseMap no albedo
uniform bool useEmissive;          // 1 = usa uEmissiveMap (so laptop)
uniform bool useVertexColor;       // 1 = usa VertexColor (TV e porta)
uniform bool uNgonPanel;           // 1 = painel losangulos_parede (emissive via rgbPanel(), nao uEmissiveMap)
uniform bool uMetallic;            // molduras NgonParede (brilho especular)
uniform float uSpecScale;          // intensidade do specular (sofa/detalhe = baixo)

// Cena (setado em FinalProject::DrawScene)
uniform float uTime;               // segundos -> anima cor dos paineis (rgbPanel)
uniform vec3 uViewPos;             // posicao da camera (specular)

// Luzes
#define MAX_NGON_LIGHTS 8
uniform int uNgonLightCount;                     // quantas luzes ngon (max 8)
uniform vec3 uNgonLightPos[MAX_NGON_LIGHTS];     // posicao de cada painel-luz (CLoadAssets); ilumina a parede (point light, nao emissive)
uniform vec3 uFillLightPos;                      // luz de preenchimento
uniform float uFillLightStrength;                // intensidade da fill

// Cor RGB ciclica dos paineis (posicao + tempo)
vec3 rgbPanel(vec3 worldPosition)
{
	float t = worldPosition.y * 0.025 + worldPosition.x * 0.012 + uTime * 0.55;
	return 0.5 + 0.5 * cos(vec3(0.0, 2.0, 4.0) + t * 6.28318);
}

// Brilho especular (superficies metalicas)
float specTerm(vec3 lightDir, vec3 norm, vec3 viewDir, float power)
{
	vec3 halfDir = normalize(lightDir + viewDir);
	return pow(max(dot(norm, halfDir), 0.0), power);
}

// Luz ngon enfraquece com a distancia
float ngonAttenuation(float dist)
{
	dist = max(dist, 8.0);
	return 1.0 / (1.0 + 0.012 * dist + 0.00018 * dist * dist);
}

// Luz fill enfraquece com a distancia (curva mais suave)
float fillAttenuation(float dist)
{
	return 1.0 / (1.0 + 0.004 * dist + 0.00004 * dist * dist);
}

void main()
{
	vec3 norm = normalize(Normal);

	// --- Cor base (albedo) + emissivo proprio ---
	vec3 albedo = objColor.rgb;
	vec3 emissive = vec3(0.0);
    if (useVertexColor)
        albedo = VertexColor;                                    // TV, porta
    else if (useMap)
        albedo *= texture(uDiffuseMap, TexCoord).rgb;            // textura diffuse
	if (useEmissive)
	{
		vec3 screenEmit = texture(uEmissiveMap, TexCoord).rgb;   // tela do laptop
		emissive += screenEmit * 1.4;
		albedo = max(albedo, screenEmit * 0.35);
	}
	else if (uNgonPanel)
	{
		albedo = rgbPanel(WorldPos);                             // painel brilhante
		emissive = albedo * 1.15;
	}

	// Luz ambiente minima (evita preto total)
	vec3 ambient = albedo * 0.32;

	vec3 pointLight = vec3(0.0);
	vec3 specular = vec3(0.0);
	vec3 viewDir = normalize(uViewPos - WorldPos);

	float specPower = 88.0;
	float specScale = 4.0 * uSpecScale;
	vec3 specTint = vec3(0.90, 0.92, 0.96);

	// --- Luzes dos paineis ngon (ate 8) ---
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

	// --- Luz de preenchimento (fill) ---
	vec3 fillDir = normalize(uFillLightPos - WorldPos);
	float fillDist = length(uFillLightPos - WorldPos);
	float fillDiff = max(dot(norm, fillDir), 0.0);
	vec3 fillLight = albedo * fillDiff * vec3(0.82, 0.85, 0.92) * uFillLightStrength * fillAttenuation(fillDist);

	// Soma todas as contribuicoes e comprime brancos estourados
	vec3 result = ambient + pointLight + fillLight + specular + emissive;
	result = result / (result + vec3(1.0));
	FragColor = vec4(result, 1.0);
}
