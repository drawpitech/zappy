/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** GBufferPass
*/

#include "GBufferPass.hpp"
#include "Window.hpp"

GBufferPass::GBufferPass(std::shared_ptr<Window>& window) : m_window(window) {
    m_staticShaderProgram = std::make_shared<ShaderProgram>("../GUI/shaders/GBufferStatic.vert", "../GUI/shaders/GBuffer.frag");
    m_skinnedShaderProgram = std::make_shared<ShaderProgram>("../GUI/shaders/GBufferSkinned.vert", "../GUI/shaders/GBuffer.frag");

    // Framebuffer creation
    glGenFramebuffers(1, &m_gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer);

    // Position texture
    glGenTextures(1, &m_positionTexture);
    glBindTexture(GL_TEXTURE_2D, m_positionTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, static_cast<int>(m_window->getWidth()), static_cast<int>(m_window->getHeight()), 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_positionTexture, 0);

    // Normal texture
    glGenTextures(1, &m_normalTexture);
    glBindTexture(GL_TEXTURE_2D, m_normalTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, static_cast<int>(m_window->getWidth()), static_cast<int>(m_window->getHeight()), 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_normalTexture, 0);

    // Albedo texture
    glGenTextures(1, &m_albedoTexture);
    glBindTexture(GL_TEXTURE_2D, m_albedoTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<int>(m_window->getWidth()), static_cast<int>(m_window->getHeight()), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_albedoTexture, 0);

    // PBR texture
    glGenTextures(1, &m_pbrTexture);
    glBindTexture(GL_TEXTURE_2D, m_pbrTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, static_cast<int>(m_window->getWidth()), static_cast<int>(m_window->getHeight()), 0, GL_RG, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_pbrTexture, 0);


    // Framebuffer attachments
    std::array<unsigned int, 4> attachments = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 , GL_COLOR_ATTACHMENT3};
    glDrawBuffers(4, attachments.data());

    // Depth buffer creation (renderbuffer)
    glGenRenderbuffers(1, &m_depthTexture);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthTexture);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, static_cast<int>(m_window->getWidth()), static_cast<int>(m_window->getHeight()));
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthTexture);

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
    glDeleteRenderbuffers(1, &m_rbo);
    glDeleteVertexArrays(1, &m_quadVAO);
    glDeleteBuffers(1, &m_quadVBO);
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
