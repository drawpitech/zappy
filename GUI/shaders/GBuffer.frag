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

void main() {
    vec4 albedo = useAlbedoMap ? texture(albedoMap, inTexCoords) : vec4(1.0);
    if (albedo.a < 0.1)
        discard;

    outAlbedo = albedo.rgb;
    outNormal = useNormalMap ? getNormalFromMap() : inNormal;
    outPbr = useMetallicRoughnessMap ? texture(metallicRoughnessMap, inTexCoords).gb : vec2(0.0);
    outFragPos = inFragPos;
}
