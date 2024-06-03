#version 460 core

layout(location = 0) out vec3 FragColor;

layout(location = 0) in vec2 inTexCoords;

uniform sampler2D normalMap;
uniform sampler2D albedoMap;
uniform sampler2D depthMap;

uniform mat4 proj;
uniform mat4 view;
uniform vec2 attachmentSize;

bool rayIsOutofScreen(vec2 ray){
    return (ray.x > 1 || ray.y > 1 || ray.x < 0 || ray.y < 0) ? true : false;
}

vec3 raymarch(vec3 rayPos, vec3 dir, int iterationCount){
    float sampleDepth;
    vec3 hitColor = vec3(0);
    bool hit = false;

    for (int i = 0; i < iterationCount; i++) {
        rayPos += dir;
        if (rayIsOutofScreen(rayPos.xy))
            break;

        sampleDepth = texture(depthMap, rayPos.xy).r;
        float depthDif = rayPos.z - sampleDepth;
        if (depthDif >= 0 && depthDif < 0.001){ //we have a hit
            hit = true;
            hitColor = texture(albedoMap, rayPos.xy).rgb;
            break;
        }
    }

    return hitColor;
}

void main() {
    float maxRayDistance = 100.0f;

    // View Space ray calculation
    vec3 pixelPositionTexture;
    pixelPositionTexture.xy = vec2(gl_FragCoord.x / attachmentSize.x,  gl_FragCoord.y / attachmentSize.y);
    vec3 normalView = (view * vec4(texture(normalMap, pixelPositionTexture.xy).rgb, 0)).xyz;

    float pixelDepth = texture(depthMap, pixelPositionTexture.xy).r;
    pixelPositionTexture.z = pixelDepth;

    vec4 positionView = inverse(proj) * vec4(pixelPositionTexture * 2 - vec3(1), 1);
    positionView /= positionView.w;
    vec3 reflectionView = normalize(reflect(positionView.xyz, normalView));
    if (reflectionView.z > 0){
        FragColor = vec3(0);
        return;
    }

    vec3 rayEndPositionView = positionView.xyz + reflectionView * maxRayDistance;


    // Texture Space ray calculation
    vec4 rayEndPositionTexture = proj * vec4(rayEndPositionView,1);
    rayEndPositionTexture /= rayEndPositionTexture.w;
    rayEndPositionTexture.xyz = (rayEndPositionTexture.xyz + vec3(1)) / 2.0f;
    vec3 rayDirectionTexture = rayEndPositionTexture.xyz - pixelPositionTexture;

    ivec2 screenSpaceStartPosition = ivec2(pixelPositionTexture.x * attachmentSize.x, pixelPositionTexture.y * attachmentSize.y);
    ivec2 screenSpaceEndPosition = ivec2(rayEndPositionTexture.x * attachmentSize.x, rayEndPositionTexture.y * attachmentSize.y);
    ivec2 screenSpaceDistance = screenSpaceEndPosition - screenSpaceStartPosition;
    int screenSpaceMaxDistance = max(abs(screenSpaceDistance.x), abs(screenSpaceDistance.y)) / 2;
    rayDirectionTexture /= max(screenSpaceMaxDistance, 0.001f);


    // trace the ray
    FragColor = raymarch(pixelPositionTexture, rayDirectionTexture, screenSpaceMaxDistance);
}
