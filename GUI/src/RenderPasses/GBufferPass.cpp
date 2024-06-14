/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** GBufferPass
*/

#include "RenderPasses/GBufferPass.hpp"
#include "Renderer/ShaderProgram.hpp"
#include "Renderer/Window.hpp"

GBufferPass::GBufferPass(std::shared_ptr<Window>& window) : m_window(window) {
    m_staticShaderProgram = std::make_shared<ShaderProgram>("../GUI/shaders/GBufferStatic.vert", "../GUI/shaders/GBuffer.frag");
    m_skinnedShaderProgram = std::make_shared<ShaderProgram>("../GUI/shaders/GBufferSkinned.vert", "../GUI/shaders/GBuffer.frag");

    // Framebuffer creation
    glGenFramebuffers(1, &m_gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer);

    // Position texture
    glGenTextures(1, &m_positionTexture);
    glBindTexture(GL_TEXTURE_2D, m_positionTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, static_cast<int>(m_window->getWidth()), static_cast<int>(m_window->getHeight()), 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_positionTexture, 0);

    // Normal texture
    glGenTextures(1, &m_normalTexture);
    glBindTexture(GL_TEXTURE_2D, m_normalTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, static_cast<int>(m_window->getWidth()), static_cast<int>(m_window->getHeight()), 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_normalTexture, 0);

    // Albedo texture
    glGenTextures(1, &m_albedoTexture);
    glBindTexture(GL_TEXTURE_2D, m_albedoTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<int>(m_window->getWidth()), static_cast<int>(m_window->getHeight()), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_albedoTexture, 0);

    // PBR texture
    glGenTextures(1, &m_pbrTexture);
    glBindTexture(GL_TEXTURE_2D, m_pbrTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, static_cast<int>(m_window->getWidth()), static_cast<int>(m_window->getHeight()), 0, GL_RG, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_pbrTexture, 0);

    // Depth buffer
    glGenTextures(1, &m_depthTexture);
    glBindTexture(GL_TEXTURE_2D, m_depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, static_cast<int>(m_window->getWidth()), static_cast<int>(m_window->getHeight()), 0, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, m_depthTexture, 0);

    // Framebuffer attachments
    std::array<unsigned int, 4> attachments = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 , GL_COLOR_ATTACHMENT3};
    glDrawBuffers(4, attachments.data());

    // Framebuffer validation
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("Framebuffer is not complete");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GBufferPass::~GBufferPass() {
    glDeleteFramebuffers(1, &m_gBuffer);
    glDeleteTextures(1, &m_positionTexture);
    glDeleteTextures(1, &m_normalTexture);
    glDeleteTextures(1, &m_albedoTexture);
    glDeleteTextures(1, &m_pbrTexture);
    glDeleteTextures(1, &m_depthTexture);
    glDeleteVertexArrays(1, &m_quadVAO);
    glDeleteBuffers(1, &m_quadVBO);
}

void GBufferPass::resize(const glm::vec2& size) const {
    glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer);

    glBindTexture(GL_TEXTURE_2D, m_positionTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, static_cast<GLsizei>(size[0]), static_cast<GLsizei>(size[1]), 0, GL_RGBA, GL_FLOAT, nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_positionTexture, 0);

    glBindTexture(GL_TEXTURE_2D, m_normalTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, static_cast<GLsizei>(size[0]), static_cast<GLsizei>(size[1]), 0, GL_RGBA, GL_FLOAT, nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_normalTexture, 0);

    glBindTexture(GL_TEXTURE_2D, m_albedoTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<GLsizei>(size[0]), static_cast<GLsizei>(size[1]), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_albedoTexture, 0);

    glBindTexture(GL_TEXTURE_2D, m_pbrTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, static_cast<GLsizei>(size[0]), static_cast<GLsizei>(size[1]), 0, GL_RG, GL_UNSIGNED_BYTE, nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_pbrTexture, 0);

    glBindTexture(GL_TEXTURE_2D, m_depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, static_cast<GLsizei>(size[0]), static_cast<GLsizei>(size[1]), 0, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, nullptr);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, m_depthTexture, 0);

    std::array<unsigned int, 4> attachments = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 , GL_COLOR_ATTACHMENT3};
    glDrawBuffers(4, attachments.data());

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("Framebuffer is not complete after resizing");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBufferPass::bindFramebuffer() const noexcept {
    glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
}

void GBufferPass::bindStaticShader(const glm::mat4& view, const glm::mat4& proj) const noexcept {
    m_staticShaderProgram->use();
    m_staticShaderProgram->setMat4("view", view);
    m_staticShaderProgram->setMat4("proj", proj);
}

void GBufferPass::bindSkinnedShader(const glm::mat4& view, const glm::mat4& proj) const noexcept {
    m_skinnedShaderProgram->use();
    m_skinnedShaderProgram->setMat4("view", view);
    m_skinnedShaderProgram->setMat4("proj", proj);
}
