#version 460 core

layout(location = 0) in vec3 inFragPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoords;

layout(location = 0) out vec4 FragColor;

uniform sampler2D albedoMap;
uniform sampler2D metallicRoughnessMap;
uniform sampler2D normalMap;
uniform sampler2D aoMap;

uniform bool useAlbedoMap;
uniform bool useMetallicRoughnessMap;
uniform bool useNormalMap;
uniform bool useAoMap;

uniform vec3 camPos;

const vec3 lightPos = vec3(0.0, 1.0, 0.0);
const vec3 lightColor = vec3(1.0, 1.0, 1.0);
const float lightIntensity = 5.0;

const float PI = 3.14159265359;

vec3 getNormalFromMap() {
    vec3 tangentNormal = texture(normalMap, inTexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(inFragPos);
    vec3 Q2  = dFdy(inFragPos);
    vec2 st1 = dFdx(inTexCoords);
    vec2 st2 = dFdy(inTexCoords);

    vec3 N   = normalize(inNormal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

float distributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float geometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float geometrySmith(vec3 N, vec3 viewDirection, vec3 L, float roughness) {
    float NdotV = max(dot(N, viewDirection), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = geometrySchlickGGX(NdotV, roughness);
    float ggx1 = geometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main() {
    vec3 albedo = vec3(1.0, 0.0, 1.0);

    vec2 texCoords = inTexCoords;
    if (useAlbedoMap)
        albedo = pow(texture(albedoMap, texCoords).rgb, vec3(2.2));

    vec3 normal = normalize(inNormal);
    if (useNormalMap)
        normal = getNormalFromMap();

    float metallic = 0.0;
    float roughness = 0.0;
    if (useMetallicRoughnessMap) {
        vec4 metallicRoughness = texture(metallicRoughnessMap, texCoords);
        metallic = metallicRoughness.b;
        roughness = metallicRoughness.g;
    }

    float ao = 1.0;
    if (useAoMap)
        ao = texture(aoMap, texCoords).r;

    // Pbr lighting
    vec3 Lo = vec3(0.0);
    {
        vec3 F0 = vec3(0.04);
        F0 = mix(F0, albedo, metallic);

        vec3 lightDir = normalize(lightPos - inFragPos);
        vec3 viewDirection = normalize(camPos - inFragPos);
        vec3 halfwayDir = normalize(lightDir + viewDirection);

        float distance = length(lightPos - inFragPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lightColor * attenuation * lightIntensity;

        // Cook-Torrance BRDF
        float normalDistribution = distributionGGX(normal, halfwayDir, roughness);
        float geometricOcculusion = geometrySmith(normal, viewDirection, lightDir, roughness);
        vec3 fresnel = fresnelSchlick(max(dot(halfwayDir, viewDirection), 0.0), F0);

        // Specular
        vec3 numerator = normalDistribution * geometricOcculusion * fresnel;
        float denominator = 4.0 * max(dot(normal, viewDirection), 0.0) * max(dot(normal, lightDir), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;

        // Diffuse
        vec3 diffuse = vec3(1.0) - fresnel;
        diffuse *= 1.0 - metallic;

        float cosTheta = max(dot(normal, lightDir), 0.0);

        Lo += (diffuse * albedo / PI + specular) * radiance * cosTheta;
    }

    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo;

    color = color / (color + vec3(1.0)); // HDR tonemapping
    color = pow(color, vec3(1.0 / 2.2)); // gamma correction
    FragColor = vec4(color, 1.0);
}
