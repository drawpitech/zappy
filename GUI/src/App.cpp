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

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"

#include <chrono>
#include <memory>

App::App() {
    m_window = std::make_shared<Window>(1280, 720, "Zappy");
    m_camera = std::make_unique<Camera>(m_window);
    m_camera->setPerspective(70, static_cast<float>(m_window->getWidth()) / static_cast<float>(m_window->getHeight()), 0.1f, 100.0f);
    m_shaderProgram = std::make_shared<ShaderProgram>("../GUI/shaders/vertex.glsl", "../GUI/shaders/fragment.glsl");

    {   // ImGui initialization
        IMGUI_CHECKVERSION();
        if (!ImGui::CreateContext() || !ImGui_ImplGlfw_InitForOpenGL(m_window->getHandle(), true) || !ImGui_ImplOpenGL3_Init("#version 460"))
            throw std::runtime_error("Failed to initialize ImGui");

        ImGui::StyleColorsDark();
    }
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
}

void App::drawUi() const noexcept {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    {   // Telemetry
        ImGui::SetNextWindowBgAlpha(0);
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(300, 200));
        ImGui::Begin("Telemetry", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground);
            ImGui::Text("Frame time: %.3f", m_deltaTime * 1000.0f);
            ImGui::Text("Frame rate: %.3f", 1 / m_deltaTime);
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

        drawUi();
        m_window->swapBuffers();
    }
}
