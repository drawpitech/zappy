/*
** EPITECH PROJECT, 2023
** zappy
** File description:
** IRenderer.hpp
*/

#pragma once

#include "Models/Animations/Animator.hpp"
#include "Models/SkeletalMesh.hpp"
#include "Models/StaticMesh.hpp"

class IRenderer {
    public:
        struct AnimatedActor {
            std::shared_ptr<SkeletalMesh> mesh;
            std::shared_ptr<Animator> animator;
            glm::vec3 position;
            glm::vec3 scale;
            glm::vec3 rotation;
            glm::vec3 color = glm::vec3(1);
        };

        struct StaticActor {
            std::shared_ptr<StaticMesh> mesh;
            glm::vec3 position;
            glm::vec3 scale;
            glm::vec3 rotation;
            glm::vec3 color = glm::vec3(1);
        };

        struct Scene {
            std::vector<StaticActor> staticActors;
            std::vector<AnimatedActor> animatedActors;
        };

        IRenderer() = default;
        virtual ~IRenderer() = default;

        IRenderer(const IRenderer& other) = delete;
        IRenderer& operator=(const IRenderer& other) = delete;

        IRenderer(IRenderer&& other) = delete;
        IRenderer& operator=(IRenderer&& other) = delete;

        [[nodiscard]] virtual bool shouldStop() const noexcept = 0;
        virtual void render(std::shared_ptr<Scene>& scene, float gameSpeed) noexcept = 0;
};
