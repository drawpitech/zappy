/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** Renderer
*/

#pragma once

#include "Renderer/IRenderer.hpp"
#include "Renderer/GlRenderer/RenderPasses/GBufferPass.hpp"
#include "Renderer/GlRenderer/RenderPasses/LightingPass.hpp"
#include "Renderer/GlRenderer/RenderPasses/SSAOPass.hpp"
#include "Renderer/GlRenderer/RenderPasses/SSRPass.hpp"
#include "Renderer/GlRenderer/Camera.hpp"
#include "Renderer/GlRenderer/Window.hpp"

#include <chrono>

class GlRenderer : public IRenderer {
    public:
        GlRenderer();
        ~GlRenderer() override;

        GlRenderer(const GlRenderer& other) = delete;
        GlRenderer& operator=(const GlRenderer& other) = delete;

        GlRenderer(GlRenderer&& other) = delete;
        GlRenderer& operator=(GlRenderer&& other) = delete;

        [[nodiscard]] virtual bool shouldStop() const noexcept override { return m_window->shouldClose(); }
        virtual void render(std::shared_ptr<IRenderer::Scene>& scene, float gameSpeed) noexcept override;

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
        bool m_useSSR = false;
};
