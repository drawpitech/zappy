/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** App
*/

#pragma once

#include "GBufferPass.hpp"
#include "LightingPass.hpp"
#include "SSAOPass.hpp"
#include "Window.hpp"
#include "ShaderProgram.hpp"
#include "Camera.hpp"

#include <chrono>

class App {
    public:
        App();
        ~App();

        App (const App&) = delete;
        App& operator=(const App&) = delete;

        void run();

    private:
        void handleUserInput() noexcept;
        void updateDeltaTime() noexcept;
        void drawUi() noexcept;

    private:
        std::shared_ptr<Window> m_window;
        std::shared_ptr<ShaderProgram> m_shaderProgram;
        std::unique_ptr<Camera> m_camera;
        std::unique_ptr<GBufferPass> m_gBufferPass;
        std::unique_ptr<SSAOPass> m_ssaoPass;
        std::unique_ptr<LightingPass> m_lightingPass;

        std::chrono::time_point<std::chrono::high_resolution_clock> m_frameStartTime = std::chrono::high_resolution_clock::now();
        std::chrono::time_point<std::chrono::high_resolution_clock> m_frameEndTime = std::chrono::high_resolution_clock::now();
        float m_deltaTime;

        bool m_useSSAO = true;
};
