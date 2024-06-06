/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** Renderer
*/

#include "Renderer/Renderer.hpp"

#include "Models/Animations/Bone.hpp"
#include "Models/SkeletalMesh.hpp"
#include "Models/StaticMesh.hpp"
#include "Renderer/Window.hpp"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"

#include <memory>
#include <stdexcept>

Renderer::Renderer() {
    m_window = std::make_shared<Window>(1280, 720, "Zappy");
    m_camera = std::make_unique<Camera>(m_window);
    m_camera->setPerspective(70, static_cast<float>(m_window->getWidth()) / static_cast<float>(m_window->getHeight()), 0.1, 100.0);

    {   // Render passes
        m_gBufferPass = std::make_unique<GBufferPass>(m_window);

        m_ssaoPass = std::make_unique<SSAOPass>(m_window);
        m_ssaoPass->resize(m_window->getWidth() / 4, m_window->getHeight() / 4);

        m_ssrPass = std::make_unique<SSRPass>(m_window);
        m_ssrPass->resize(m_window->getWidth(), m_window->getHeight());

        m_lightingPass = std::make_unique<LightingPass>(m_window);
    }

    {   // ImGui initialization
        IMGUI_CHECKVERSION();
        if ((ImGui::CreateContext() == nullptr) || !ImGui_ImplGlfw_InitForOpenGL(m_window->getHandle(), true) || !ImGui_ImplOpenGL3_Init("#version 460"))
            throw std::runtime_error("Failed to initialize ImGui");

        ImGui::StyleColorsDark();
    }
}

Renderer::~Renderer() {
}

void Renderer::handleUserInput() noexcept {
    m_window->pollEvents();

    if (m_window->wasResized) {
        m_camera->setPerspective(70, static_cast<float>(m_window->getWidth()) / static_cast<float>(m_window->getHeight()), 0.1, 100.0);
        m_gBufferPass->resize(m_window->getWidth(), m_window->getHeight());
        m_ssaoPass->resize(m_window->getWidth() / 4, m_window->getHeight() / 4);
        m_ssrPass->resize(m_window->getWidth(), m_window->getHeight());
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

void Renderer::drawUi() noexcept {
    ImGui::SetNextWindowBgAlpha(0);
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(300, 300));
    ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground);

        {   // Telemetry
            ImGui::Text("Frame time: %.3f", m_deltaTime * 1000.0); // NOLINT
            ImGui::Text("Frame rate: %.3f", 1 / m_deltaTime); // NOLINT
            ImGui::Checkbox("Use SSAO", &m_useSSAO);
            ImGui::Checkbox("Use SSR", &m_useSSR);
        }

        {   // Debug view
            ImGui::Separator();
            ImGui::Text("Debug view");
            ImGui::RadioButton("Final", &m_debugView, 0);
            ImGui::RadioButton("Albedo", &m_debugView, 1);
            ImGui::RadioButton("AO", &m_debugView, 2);
            ImGui::RadioButton("SSR", &m_debugView, 3);
            ImGui::RadioButton("Normal", &m_debugView, 4);
            ImGui::RadioButton("Position", &m_debugView, 5);
            ImGui::RadioButton("Metallic", &m_debugView, 6);
            ImGui::RadioButton("Roughness", &m_debugView, 7);
        }

    ImGui::End();
}

void Renderer::updateDeltaTime() noexcept {
    m_frameEndTime = std::chrono::high_resolution_clock::now();
    m_deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(m_frameEndTime - m_frameStartTime).count();
    m_frameStartTime = m_frameEndTime;
}

void Renderer::render(std::shared_ptr<Renderer::Scene>& scene) noexcept {
    updateDeltaTime();
    handleUserInput();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    {   // Static meshes
        m_gBufferPass->bindFramebuffer();
        m_gBufferPass->bindStaticShader(m_camera->getViewMatrix(), m_camera->getProjectionMatrix());
        for (const auto& mesh : scene->staticMeshes)
            mesh.draw(m_gBufferPass->getStaticShaderProgram(), glm::mat4(1));
    }

    {   // Skeletal meshes
        m_gBufferPass->bindSkinnedShader(m_camera->getViewMatrix(), m_camera->getProjectionMatrix());


        for (auto& animatedMesh : scene->animatedMeshes) {
            animatedMesh->animator.updateAnimation(m_deltaTime);

            std::array<glm::mat4, MAX_BONES> transforms = animatedMesh->animator.getFinalBoneMatrices();
            for (int i = 0; i < transforms.size(); ++i)
                m_gBufferPass->getSkinnedShaderProgram()->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);  // NOLINT

            animatedMesh->mesh->draw(m_gBufferPass->getSkinnedShaderProgram(), glm::scale(glm::mat4(1), glm::vec3(100)));
        }

            // animator.updateAnimation(m_deltaTime / 2);
            // std::array<glm::mat4, MAX_BONES> transforms = animator.getFinalBoneMatrices();
            // for (int i = 0; i < transforms.size(); ++i)
                // m_gBufferPass->getSkinnedShaderProgram()->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);  // NOLINT
        // dan->draw(m_gBufferPass->getSkinnedShaderProgram(), glm::mat4(1));
    }

    if (m_useSSAO) {
        m_ssaoPass->bindMainPass(
            m_gBufferPass->getPositionTexture(),
            m_gBufferPass->getNormalTexture(),
            m_camera->getViewMatrix(),
            m_camera->getProjectionMatrix()
        );
        Utils::renderQuad();
        m_ssaoPass->bindBlurPass();
        Utils::renderQuad();
    } else {
        glBindFramebuffer(GL_FRAMEBUFFER, m_ssaoPass->getBluredFramebuffer());
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    if (m_useSSR) {
        m_ssrPass->bind(
            m_gBufferPass->getNormalTexture(),
            m_gBufferPass->getAlbedoTexture(),
            m_gBufferPass->getDepthTexture(),
            m_camera->getProjectionMatrix(),
            m_camera->getViewMatrix()
        );
        Utils::renderQuad();
    } else {
        glBindFramebuffer(GL_FRAMEBUFFER, m_ssrPass->getFramebuffer());
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    m_lightingPass->bind(
        m_gBufferPass->getPositionTexture(),
        m_gBufferPass->getAlbedoTexture(),
        m_gBufferPass->getNormalTexture(),
        m_gBufferPass->getPbrTexture(),
        m_ssaoPass->getSSAOBlurTexture(),
        m_ssrPass->getSSRTexture(),
        m_debugView
    );
    Utils::renderQuad();

    drawUi();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    m_window->swapBuffers();
}
