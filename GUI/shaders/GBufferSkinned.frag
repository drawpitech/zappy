#version 460

layout(location = 0) out vec3 outFragPos;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outAlbedo;
layout(location = 3) out vec2 outPbr;

layout(location = 0) in vec3 inFragPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoords;

uniform sampler2D albedoMap;
uniform sampler2D metallicRoughnessMap;
uniform sampler2D normalMap;

uniform bool useAlbedoMap;
uniform bool useMetallicRoughnessMap;
uniform bool useNormalMap;

void main() {
    vec4 albedo = useAlbedoMap ? texture(albedoMap, inTexCoords) : vec4(1.0);
    if (albedo.a < 0.1)
        discard;

    outAlbedo = albedo.rgb;
    outNormal = normalize(inNormal);
    outPbr = useMetallicRoughnessMap ? texture(metallicRoughnessMap, inTexCoords).gb : vec2(0.0);
    outFragPos = vec3(0, 0, 0);
}
