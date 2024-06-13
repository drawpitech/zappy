#version 460 core

layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec3 inWorldPos;

uniform samplerCube envMap;
uniform sampler2D normalMap;

void main() {
    vec3 normal = texture(normalMap, gl_FragCoord.xy / vec2(1280, 720)).xyz;
    if (length(normal) > 0.1)
        discard;

    vec3 envColor = texture(envMap, inWorldPos).rgb;

    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0/2.2));

    FragColor = vec4(envColor, 1.0);
}
