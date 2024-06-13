/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** Bone
*/

#pragma once

#define MAX_BONES 100

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <assimp/scene.h>

#include <vector>

struct KeyPosition {
    glm::vec3 position;
    float timeStamp;
};

struct KeyRotation {
    glm::quat orientation;
    float timeStamp;
};

struct KeyScale {
    glm::vec3 scale;
    float timeStamp;
};

class Bone {
    public:
        Bone(std::string  name, int id, const aiNodeAnim* channel);
        ~Bone() = default;

        Bone(const Bone& other) = default;
        Bone& operator=(const Bone& other) = default;

        Bone(Bone&& other) noexcept = default;
        Bone& operator=(Bone&& other) noexcept = default;

        void update(float animationTime);

        [[nodiscard]] const glm::mat4& getLocalTransform() const { return m_localTransform; }
        [[nodiscard]] const std::string& getBoneName() const { return m_name; }
        [[nodiscard]] int getBoneID() const { return m_id; }
        [[nodiscard]] int getPositionIndex(float animationTime) const;
        [[nodiscard]] int getRotationIndex(float animationTime) const;
        [[nodiscard]] int getScaleIndex(float animationTime) const;

    private:
        [[nodiscard]] static float getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);
        [[nodiscard]] glm::mat4 interpolatePosition(float animationTime) const;
        [[nodiscard]] glm::mat4 interpolateRotation(float animationTime) const;
        [[nodiscard]] glm::mat4 interpolateScaling(float animationTime) const;

        std::vector<KeyPosition> m_positions;
        std::vector<KeyRotation> m_rotations;
        std::vector<KeyScale> m_scales;
        unsigned int m_numPositions;
        unsigned int m_numRotations;
        unsigned int m_numScalings;

        glm::mat4 m_localTransform;
        std::string m_name;
        int m_id;
};
