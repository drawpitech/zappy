#version 460 core

layout(location = 0) in vec3 inPos;

layout(location = 0) out vec3 outWorldPos;

uniform mat4 proj;
uniform mat4 view;

void main() {
    outWorldPos = inPos;

    mat4 rotView = mat4(mat3(view));
    vec4 clipPos = proj * rotView * vec4(outWorldPos, 1.0);

    gl_Position = clipPos.xyww;
}
