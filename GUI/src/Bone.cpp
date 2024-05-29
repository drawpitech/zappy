/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** Bone
*/

#include "Bone.hpp"

#include <stdexcept>

Bone::Bone(std::string  name, int id, const aiNodeAnim* channel) : m_name(std::move(name)), m_id(id), m_numPositions(channel->mNumPositionKeys), m_localTransform(1.0) {
    for (int positionIndex = 0; positionIndex < m_numPositions; ++positionIndex) {
        aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
        float timeStamp = static_cast<float>(channel->mPositionKeys[positionIndex].mTime);
        KeyPosition data = {
            .position = glm::vec3(aiPosition.x, aiPosition.y, aiPosition.z),
            .timeStamp = timeStamp
        };

        m_positions.push_back(data);
    }

    m_numRotations = channel->mNumRotationKeys;
    for (int rotationIndex = 0; rotationIndex < m_numRotations; ++rotationIndex) {
        aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
        float timeStamp = static_cast<float>(channel->mRotationKeys[rotationIndex].mTime);
        KeyRotation data = {
            .orientation = glm::quat(aiOrientation.w, aiOrientation.x, aiOrientation.y, aiOrientation.z),
            .timeStamp = timeStamp
        };

        m_rotations.push_back(data);
    }

    m_numScalings = channel->mNumScalingKeys;
    for (int keyIndex = 0; keyIndex < m_numScalings; ++keyIndex) {
        aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
        float timeStamp = static_cast<float>(channel->mScalingKeys[keyIndex].mTime);
        KeyScale data = {
            .scale = glm::vec3(scale.x, scale.y, scale.z),
            .timeStamp = timeStamp
        };

        m_scales.push_back(data);
    }
}

void Bone::update(float animationTime) {
    glm::mat4 translation = interpolatePosition(animationTime);
    glm::mat4 rotation = interpolateRotation(animationTime);
    glm::mat4 scale = interpolateScaling(animationTime);
    m_localTransform = translation * rotation * scale;
}

int Bone::getPositionIndex(float animationTime) const {
    for (int index = 0; index < m_numPositions - 1; ++index)
        if (animationTime < m_positions[index + 1].timeStamp)
            return index;

    throw std::runtime_error("bruh");
}

int Bone::getRotationIndex(float animationTime) const {
    for (int index = 0; index < m_numRotations - 1; ++index)
        if (animationTime < m_rotations[index + 1].timeStamp)
            return index;

    throw std::runtime_error("bruh");
}

int Bone::getScaleIndex(float animationTime) const {
    for (int index = 0; index < m_numScalings - 1; ++index)
        if (animationTime < m_scales[index + 1].timeStamp)
            return index;

    throw std::runtime_error("bruh");
}

float Bone::getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) {
    float scaleFactor = 0.0;
    float midWayLength = animationTime - lastTimeStamp;
    float framesDiff = nextTimeStamp - lastTimeStamp;
    scaleFactor = midWayLength / framesDiff;

    return scaleFactor;
}

glm::mat4 Bone::interpolatePosition(float animationTime) const {
    if (1 == m_numPositions)
        return glm::translate(glm::mat4(1.0), m_positions[0].position);

    int p0Index = getPositionIndex(animationTime);
    int p1Index = p0Index + 1;
    float scaleFactor = getScaleFactor(m_positions[p0Index].timeStamp, m_positions[p1Index].timeStamp, animationTime);
    glm::vec3 finalPosition = glm::mix(m_positions[p0Index].position, m_positions[p1Index].position, scaleFactor);

    return glm::translate(glm::mat4(1.0), finalPosition);
}

glm::mat4 Bone::interpolateRotation(float animationTime) const {
    if (1 == m_numRotations) {
        auto rotation = glm::normalize(m_rotations[0].orientation);
        return glm::toMat4(rotation);
    }

    int p0Index = getRotationIndex(animationTime);
    int p1Index = p0Index + 1;
    float scaleFactor = getScaleFactor(m_rotations[p0Index].timeStamp, m_rotations[p1Index].timeStamp, animationTime);
    glm::quat finalRotation = glm::slerp(m_rotations[p0Index].orientation, m_rotations[p1Index].orientation, scaleFactor);
    finalRotation = glm::normalize(finalRotation);

    return glm::toMat4(finalRotation);
}

glm::mat4 Bone::interpolateScaling(float animationTime) const {
    if (1 == m_numScalings)
        return glm::scale(glm::mat4(1.0), m_scales[0].scale);

    int p0Index = getScaleIndex(animationTime);
    int p1Index = p0Index + 1;
    float scaleFactor = getScaleFactor(m_scales[p0Index].timeStamp, m_scales[p1Index].timeStamp, animationTime);
    glm::vec3 finalScale = glm::mix(m_scales[p0Index].scale, m_scales[p1Index].scale, scaleFactor);

    return glm::scale(glm::mat4(1.0), finalScale);
}
