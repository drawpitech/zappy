/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** App
*/

#pragma once

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

    private:
        std::shared_ptr<Window> m_window;
        std::shared_ptr<ShaderProgram> m_shaderProgram;
        std::shared_ptr<Camera> m_camera;

        std::chrono::time_point<std::chrono::high_resolution_clock> m_frameStartTime = std::chrono::high_resolution_clock::now();
        std::chrono::time_point<std::chrono::high_resolution_clock> m_frameEndTime = std::chrono::high_resolution_clock::now();
        float m_deltaTime;
};
