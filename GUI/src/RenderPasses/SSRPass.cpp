/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** SSRPass
*/

#include "RenderPasses/SSRPass.hpp"
#include "Renderer/ShaderProgram.hpp"

#include <stdexcept>

SSRPass::SSRPass(std::shared_ptr<Window>& window) : m_window(window), m_size(glm::vec2(window->getWidth(), window->getHeight())) {
    m_shaderProgram = std::make_unique<ShaderProgram>("../GUI/shaders/Lighting.vert", "../GUI/shaders/SSR.frag");

    // Framebuffer creation
    glGenFramebuffers(1, &m_ssrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_ssrFBO);

    // Texture creation
    glGenTextures(1, &m_ssrTexture);
    glBindTexture(GL_TEXTURE_2D, m_ssrTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, static_cast<int>(m_window->getWidth()), static_cast<int>(m_window->getHeight()), 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ssrTexture, 0);

    // Framebuffer attachments
    std::array<unsigned int, 1> attachments = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, attachments.data());

    // Framebuffer validation
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("Framebuffer is not complete");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

SSRPass::~SSRPass() {
    glDeleteFramebuffers(1, &m_ssrFBO);
    glDeleteTextures(1, &m_ssrTexture);
}

void SSRPass::resize(const glm::vec2& size) noexcept {
    glBindTexture(GL_TEXTURE_2D, m_ssrTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, static_cast<int>(size[0]), static_cast<int>(size[1]), 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    m_size = glm::vec2(size[0], size[1]);
}

void SSRPass::bind(uint32_t gBufferNormal, uint32_t gBufferAlbedo, uint32_t gBufferDepth, const glm::mat4& proj, const glm::mat4& view) const noexcept {
    m_shaderProgram->use();
    m_shaderProgram->setMat4("proj", proj);
    m_shaderProgram->setMat4("view", view);
    m_shaderProgram->setVec2("attachmentSize", m_size);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gBufferNormal);
    m_shaderProgram->setInt("normalMap", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gBufferAlbedo);
    m_shaderProgram->setInt("albedoMap", 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gBufferDepth);
    m_shaderProgram->setInt("depthMap", 2);

    glViewport(0, 0, static_cast<GLsizei>(m_size[0]), static_cast<GLsizei>(m_size[1]));
    glBindFramebuffer(GL_FRAMEBUFFER, m_ssrFBO);
    glClear(GL_COLOR_BUFFER_BIT);
}
