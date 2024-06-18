#version 460 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoords;
layout(location = 3) in ivec4 inBoneIDs;
layout(location = 4) in vec4 inWeights;

layout(location = 0) out vec3 outFragPos;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

void main() {
    vec4 totalPosition = vec4(0.0f);
    vec3 totalNormal = vec3(0.0f);
    for(int i = 0; i < MAX_BONE_INFLUENCE; i++) {
        if (inBoneIDs[i] == -1)
            continue;
        if (inBoneIDs[i] >= MAX_BONES) {
            totalPosition = vec4(inPos, 1.0f);
            break;
        }

        vec4 localPosition = finalBonesMatrices[inBoneIDs[i]] * vec4(inPos, 1.0f);
        totalPosition += localPosition * inWeights[i];
        vec3 localNormal = mat3(finalBonesMatrices[inBoneIDs[i]]) * inNormal;
        totalNormal += localNormal * inWeights[i];
    }

    mat4 viewModel = view * model;
    gl_Position =  proj * viewModel * totalPosition;
    outTexCoords = inTexCoords;
    outFragPos = vec3(model * totalPosition);
    outNormal = normalize(mat3(transpose(inverse(model))) * totalNormal);
}
