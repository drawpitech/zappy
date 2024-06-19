#version 460 core

layout(location = 0) in vec3 inPos;

layout(location = 0) out vec3 WorldPos;

uniform mat4 proj;
uniform mat4 view;

void main() {
    WorldPos = inPos;
    gl_Position =  proj * view * vec4(WorldPos, 1.0);
}
