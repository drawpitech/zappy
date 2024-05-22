/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** App
*/

#include "App.hpp"
#include "Model.hpp"
#include "ShaderProgram.hpp"
#include "Window.hpp"

#include "glm/fwd.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "imgui.h"

#include <chrono>
#include <memory>
#include <iostream>

App::App() {
    m_window = std::make_shared<Window>(1280, 720, "Zappy");
    m_camera = std::make_unique<Camera>(m_window);
    m_camera->setPerspective(70, static_cast<float>(m_window->getWidth()) / static_cast<float>(m_window->getHeight()), 0.1f, 100.0f);
    m_shaderProgram = std::make_shared<ShaderProgram>("../GUI/shaders/vertex.glsl", "../GUI/shaders/fragment.glsl");
}

App::~App() {
}

void App::handleUserInput() noexcept {
    m_window->pollEvents();

    if (m_window->wasResized) {
        m_camera->setPerspective(70, static_cast<float>(m_window->getWidth()) / static_cast<float>(m_window->getHeight()), 0.1f, 100.0f);
        m_window->wasResized = false;
    }

    if (glfwGetMouseButton(m_window->getHandle(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        m_window->showCursor(false);
        m_camera->disableCursorCallback(false);
    } else {
        m_camera->disableCursorCallback(true);
        m_window->showCursor(true);
        m_camera->resetMousePosition();
    }

    m_camera->update(m_deltaTime);
}

void App::updateDeltaTime() noexcept {
    m_frameEndTime = std::chrono::high_resolution_clock::now();
    m_deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(m_frameEndTime - m_frameStartTime).count();
    m_frameStartTime = m_frameEndTime;

    std::cout << "FPS: " << 1.0f / m_deltaTime << "\r" << std::flush;
}

void App::run() {
    Model sponza("../assets/SponzaPBR/Sponza.gltf");

    while (!m_window->shouldClose()) {
        updateDeltaTime();
        handleUserInput();

        m_window->clear();
        m_shaderProgram->use();
        m_shaderProgram->setMat4("view", m_camera->getViewMatrix());
        m_shaderProgram->setMat4("proj", m_camera->getProjectionMatrix());
        sponza.draw(m_shaderProgram);

        ImGui::ShowDemoWindow();

        m_window->swapBuffers();
    }
}
s