/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** Renderer
*/

#pragma once

#include "Models/Animations/Animator.hpp"
#include "RenderPasses/GBufferPass.hpp"
#include "RenderPasses/LightingPass.hpp"
#include "RenderPasses/SSAOPass.hpp"
#include "RenderPasses/SSRPass.hpp"
#include "Renderer/Camera.hpp"
#include "Renderer/Window.hpp"
#include "Models/SkeletalMesh.hpp"
#include "Models/StaticMesh.hpp"

#include <chrono>

class Renderer {
    public:
        struct AnimatedMesh {
            std::shared_ptr<SkeletalMesh> mesh;
            Animator animator;
        };

        struct Scene {
            std::vector<std::shared_ptr<StaticMesh>> staticMeshes;
            std::vector<std::shared_ptr<AnimatedMesh>> animatedMeshes;
        };

        Renderer();
        ~Renderer();

        Renderer(const Renderer& other) = delete;
        Renderer& operator=(const Renderer& other) = delete;

        Renderer(Renderer&& other) = delete;
        Renderer& operator=(Renderer&& other) = delete;

        [[nodiscard]] bool shouldStop() const noexcept { return m_window->shouldClose(); }
        void render(std::shared_ptr<Renderer::Scene>& scene, float gameSpeed) noexcept;

    private:
        void handleUserInput() noexcept;
        void drawUi() noexcept;

        void updateDeltaTime() noexcept;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_frameStartTime = std::chrono::high_resolution_clock::now();
        std::chrono::time_point<std::chrono::high_resolution_clock> m_frameEndTime = std::chrono::high_resolution_clock::now();
        float m_deltaTime = 0;

        std::shared_ptr<Window> m_window;
        std::unique_ptr<Camera> m_camera;

        std::unique_ptr<GBufferPass> m_gBufferPass;
        std::unique_ptr<SSAOPass> m_ssaoPass;
        std::unique_ptr<LightingPass> m_lightingPass;
        std::unique_ptr<SSRPass> m_ssrPass;

        int m_debugView = 0;
        bool m_useSSAO = true;
        bool m_useSSR = true;
};
