#version 460

layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec2 inTexCoords;

uniform sampler2D positionMap;
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D pbrMap;
uniform sampler2D ssaoMap;

uniform vec3 camPos;
uniform bool useSSAO;

const vec3 lightPos = vec3(0.0, 1.0, 0.0);
const vec3 lightColor = vec3(1.0, 1.0, 1.0);
const float lightIntensity = 5.0;

const float PI = 3.14159265359;

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
    vec3 fragPos = texture(positionMap, inTexCoords).xyz;
    vec3 albedo = texture(albedoMap, inTexCoords).rgb;
    vec3 normal = normalize(texture(normalMap, inTexCoords).rgb);
    vec2 pbr = texture(pbrMap, inTexCoords).rg;
    float ssao = texture(ssaoMap, inTexCoords).r;
    float roughness = pbr.r;
    float metallic = pbr.g;

    // Pbr lighting
    vec3 Lo = vec3(0.0);
    {
        vec3 F0 = vec3(0.04);
        F0 = mix(F0, albedo, metallic);

        vec3 lightDir = normalize(lightPos - fragPos);
        vec3 viewDirection = normalize(camPos - fragPos);
        vec3 halfwayDir = normalize(lightDir + viewDirection);

        float distance = length(lightPos - fragPos);
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

    vec3 ambient = vec3(0.8) * albedo * ssao;
    vec3 color = ambient + Lo;

    color = color / (color + vec3(1.0)); // HDR tonemapping
    color = pow(color, vec3(1.0 / 2.2)); // gamma correction
    FragColor = vec4(color, 1.0);
}
