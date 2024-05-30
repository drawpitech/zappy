/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** Animator
*/

#pragma once

#include "Animation.hpp"
#include "Bone.hpp"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>

class Animator {
    public:
        Animator(std::shared_ptr<Animation> animation);
        ~Animator() = default;

        Animator(const Animator& other) = delete;
        Animator& operator=(const Animator& other) = delete;

        Animator(Animator&& other) = delete;
        Animator& operator=(Animator&& other) = delete;

        void updateAnimation(float deltaTime) noexcept;
        void playAnimation(const std::shared_ptr<Animation>& animation) noexcept;
        void calculateBoneTransform(const AssimpNodeData& node, glm::mat4 parentTransform) noexcept;

        [[nodiscard]] std::array<glm::mat4, MAX_BONES> getFinalBoneMatrices() const noexcept;

    private:
        std::array<glm::mat4, MAX_BONES> m_finalBoneMatrices = {glm::mat4(1)};
        std::shared_ptr<Animation> m_currentAnimation;
        float m_currentTime = 0.0;
        float m_deltaTime = 0.0;
};
