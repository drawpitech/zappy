/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** App
*/

#include "App.hpp"
#include "GlUtils.hpp"
#include "LightingPass.hpp"
#include "Model.hpp"
#include "SSAOPass.hpp"
#include "Window.hpp"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"

#include <chrono>
#include <memory>

App::App() {
    m_window = std::make_shared<Window>(1280, 720, "Zappy");
    m_camera = std::make_unique<Camera>(m_window);
    m_camera->setPerspective(70, static_cast<float>(m_window->getWidth()) / static_cast<float>(m_window->getHeight()), 0.1, 100.0);
    m_gBufferPass = std::make_unique<GBufferPass>(m_window);
    m_lightingPass = std::make_unique<LightingPass>(m_window);
    m_ssaoPass = std::make_unique<SSAOPass>(m_window);

    {   // ImGui initialization
        IMGUI_CHECKVERSION();
        if ((ImGui::CreateContext() == nullptr) || !ImGui_ImplGlfw_InitForOpenGL(m_window->getHandle(), true) || !ImGui_ImplOpenGL3_Init("#version 460"))
            throw std::runtime_error("Failed to initialize ImGui");

        ImGui::StyleColorsDark();
    }
}

App::~App() {
}

void App::handleUserInput() noexcept {
    m_window->pollEvents();

    if (m_window->wasResized) {
        m_camera->setPerspective(70, static_cast<float>(m_window->getWidth()) / static_cast<float>(m_window->getHeight()), 0.1, 100.0);
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

void App::drawUi() noexcept {
    ImGui::SetNextWindowBgAlpha(0);
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(300, 200));
    ImGui::Begin("Telemetry", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground);
        ImGui::Text("Frame time: %.3f", m_deltaTime * 1000.0); // NOLINT
        ImGui::Text("Frame rate: %.3f", 1 / m_deltaTime); // NOLINT
        ImGui::Checkbox("Use SSAO", &m_useSSAO);
    ImGui::End();
}

void App::run() {
    Model sponza("../assets/SponzaPBR/Sponza.gltf");

    while (!m_window->shouldClose()) {
        updateDeltaTime();
        handleUserInput();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        m_gBufferPass->bind(glm::mat4(1.0), m_camera->getViewMatrix(), m_camera->getProjectionMatrix());
        sponza.draw(m_gBufferPass->getShaderProgram());

        if (m_useSSAO) {
            m_ssaoPass->bindMainPass(
                m_gBufferPass->getPositionTexture(),
                m_gBufferPass->getNormalTexture(),
                m_camera->getViewMatrix(),
                m_camera->getProjectionMatrix()
            );
            GlUtils::renderQuad();
            m_ssaoPass->bindBlurPass();
            GlUtils::renderQuad();
        } else {
            glBindFramebuffer(GL_FRAMEBUFFER, m_ssaoPass->getBluredFramebuffer());
            glClearColor(1.0, 1.0, 1.0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        m_lightingPass->bind(
            m_gBufferPass->getPositionTexture(),
            m_gBufferPass->getAlbedoTexture(),
            m_gBufferPass->getNormalTexture(),
            m_gBufferPass->getPbrTexture(),
            m_ssaoPass->getSSAOBlurTexture()
        );
        GlUtils::renderQuad();

        drawUi();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        m_window->swapBuffers();
    }
}
