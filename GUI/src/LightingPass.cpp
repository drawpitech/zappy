/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** LightingPass
*/

#include "LightingPass.hpp"

LightingPass::LightingPass(std::shared_ptr<Window>& window) : m_window(window) {
    m_shaderProgram = std::make_unique<ShaderProgram>("../GUI/shaders/Lighting.vert", "../GUI/shaders/Lighting.frag");
}

LightingPass::~LightingPass() {
}

void LightingPass::bind(uint32_t positionTexture, uint32_t albedoTexture, uint32_t normalTexture, uint32_t pbrTexture, uint32_t ssaoTexture, int debugView) const noexcept {
    m_shaderProgram->use();

    m_shaderProgram->setInt("debugView", debugView);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, positionTexture);
    m_shaderProgram->setInt("positionMap", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, albedoTexture);
    m_shaderProgram->setInt("albedoMap", 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, normalTexture);
    m_shaderProgram->setInt("normalMap", 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, pbrTexture);
    m_shaderProgram->setInt("pbrMap", 3);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, ssaoTexture);
    m_shaderProgram->setInt("ssaoMap", 4);

    glViewport(0, 0, m_window->getWidth(), m_window->getHeight());
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
