#version 460 core

layout(location = 0) in vec3 inFragPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoords;
layout(location = 3) in float inDistance;

layout(location = 0) out vec4 FragColor;

void main() {
    FragColor = vec4(inDistance, inDistance, inDistance, 1.0);
}
