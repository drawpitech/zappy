/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** GBufferPass
*/

#include "GBufferPass.hpp"
#include "Window.hpp"

GBufferPass::GBufferPass(std::shared_ptr<Window> window) : m_window(window) {
    m_shaderProgram = std::make_shared<ShaderProgram>("../GUI/shaders/GBuffer.vert", "../GUI/shaders/GBuffer.frag");

    // Framebuffer creation
    glGenFramebuffers(1, &m_gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer);

    // Position texture
    glGenTextures(1, &m_positionTexture);
    glBindTexture(GL_TEXTURE_2D, m_positionTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_window->getWidth(), m_window->getHeight(), 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_positionTexture, 0);

    // Normal texture
    glGenTextures(1, &m_normalTexture);
    glBindTexture(GL_TEXTURE_2D, m_normalTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_window->getWidth(), m_window->getHeight(), 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_normalTexture, 0);

    // Albedo texture
    glGenTextures(1, &m_albedoTexture);
    glBindTexture(GL_TEXTURE_2D, m_albedoTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_window->getWidth(), m_window->getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_albedoTexture, 0);

    // PBR texture
    glGenTextures(1, &m_pbrTexture);
    glBindTexture(GL_TEXTURE_2D, m_pbrTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, m_window->getWidth(), m_window->getHeight(), 0, GL_RG, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_pbrTexture, 0);


    // Framebuffer attachments
    unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 , GL_COLOR_ATTACHMENT3};
    glDrawBuffers(4, attachments);

    // Depth buffer creation (renderbuffer)
    glGenRenderbuffers(1, &m_depthTexture);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthTexture);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_window->getWidth(), m_window->getHeight());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthTexture);

    // Framebuffer validation
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("Framebuffer is not complete");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // Quad mesh
    float quadVertices[] = {
        // positions            // texture Coords
        -1.0f,  1.0f, 0.0f,     0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f,     0.0f, 0.0f,
         1.0f,  1.0f, 0.0f,     1.0f, 1.0f,
         1.0f, -1.0f, 0.0f,     1.0f, 0.0f,
    };

    glGenVertexArrays(1, &m_quadVAO);
    glGenBuffers(1, &m_quadVBO);
    glBindVertexArray(m_quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(m_quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
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

void GBufferPass::bind(const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj) const noexcept {
    glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    m_shaderProgram->use();
    m_shaderProgram->setMat4("model", model);
    m_shaderProgram->setMat4("view", view);
    m_shaderProgram->setMat4("proj", proj);
}

void GBufferPass::renderQuad() const noexcept {
    glBindVertexArray(m_quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}
