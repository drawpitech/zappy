#version 460 core

layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec3 inWorldPos;

uniform sampler2D equirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v) {
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

vec3 ACESFilm(vec3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x*(a*x + b)) / (x*(c*x + d) + e), 0.0f, 1.0f);
}

void main() {
    vec2 uv = SampleSphericalMap(normalize(inWorldPos));
    vec3 color = texture(equirectangularMap, uv).rgb;

    color = ACESFilm(color);    // HDR tonemapping
    FragColor = vec4(color, 1.0);
}
