#version 460

layout(location = 0) out float FragColor;

layout(location = 0) in vec2 inTexCoords;

uniform sampler2D positionMap;
uniform sampler2D normalMap;
uniform sampler2D noiseMap;

uniform mat4 proj;
uniform mat4 view;
uniform vec3 samples[64];
uniform vec2 attachmentSize;

const int kernelSize = 64;
const float radius = 0.5;
const float bias = 0.025;

void main() {
    const vec2 noiseScale = vec2(attachmentSize.x / 4.0, attachmentSize.y / 4.0);

    vec3 normal = mat3(view) * texture(normalMap, inTexCoords).rgb;
    if (length(normal) == 0.0) {
        FragColor = 1.0;
        return;
    }

    vec3 fragPos = vec3(view * vec4(texture(positionMap, inTexCoords).xyz, 1.0));
    if (length(fragPos) > 15) {
        FragColor = 1.0;
        return;
    }

    vec3 randomVec = normalize(texture(noiseMap, inTexCoords * noiseScale).xyz);

    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    float occlusion = 0.0;

    for(int i = 0; i < kernelSize; ++i) {
        vec3 samplePos = TBN * samples[i];
        samplePos = fragPos + samplePos * radius;

        vec4 offset = vec4(samplePos, 1.0);
        offset = proj * offset;
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;

        vec3 sampleFragPos = vec3(view * vec4(texture(positionMap, offset.xy).xyz, 1.0));
        float sampleDepth = sampleFragPos.z;

        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
    }

    occlusion = 1.0 - (occlusion / kernelSize);
    FragColor = occlusion;
}
