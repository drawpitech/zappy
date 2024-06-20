/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** SSAOPass
*/

#include "RenderPasses/SSAOPass.hpp"
#include "Renderer/ShaderProgram.hpp"

#include <random>

float lerp(float a, float b, float f) {
    return a + f * (b - a);
}

SSAOPass::SSAOPass(std::shared_ptr<Window>& window) : m_window(window), m_size(glm::vec2(window->getWidth(), window->getHeight())) {
    m_ssaoPass = std::make_unique<ShaderProgram>("GUI/shaders/Lighting.vert", "GUI/shaders/SSAO.frag");
    m_blurPass = std::make_unique<ShaderProgram>("GUI/shaders/Lighting.vert", "GUI/shaders/SSAOBlur.frag");

    glGenFramebuffers(1, &m_ssaoFBO);
    glGenFramebuffers(1, &m_ssaoBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_ssaoFBO);


    // SSAO framebuffer
    glGenTextures(1, &m_ssaoTexture);
    glBindTexture(GL_TEXTURE_2D, m_ssaoTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, static_cast<GLsizei>(window->getWidth()), static_cast<GLsizei>(window->getHeight()), 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ssaoTexture, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("Failed to create SSAO framebuffer");


    // SSAO blur framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_ssaoBlurFBO);
    glGenTextures(1, &m_ssaoBlurTexture);
    glBindTexture(GL_TEXTURE_2D, m_ssaoBlurTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, static_cast<GLsizei>(window->getWidth()), static_cast<GLsizei>(window->getHeight()), 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ssaoBlurTexture, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("Failed to create SSAO blur framebuffer");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // Generate kernel samples
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
    std::default_random_engine generator;

    for (unsigned int i = 0; i < 64; ++i) {
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        auto scale = static_cast<float>(i / 64.0);

        // scale samples s.t. they're more aligned to center of kernel
        scale = lerp(0.1, 1.0, scale * scale);
        sample *= scale;
        m_ssaoKernel.push_back(sample);
    }


    // Generate noise texture
    std::vector<glm::vec3> ssaoNoise;
    for (unsigned int i = 0; i < 16; i++) {
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0);
        ssaoNoise.push_back(noise);
    }

    glGenTextures(1, &m_noiseTexture);
    glBindTexture(GL_TEXTURE_2D, m_noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, ssaoNoise.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

SSAOPass::~SSAOPass() {
    glDeleteFramebuffers(1, &m_ssaoFBO);
    glDeleteFramebuffers(1, &m_ssaoBlurFBO);
    glDeleteTextures(1, &m_ssaoTexture);
    glDeleteTextures(1, &m_ssaoBlurTexture);
}

void SSAOPass::resize(const glm::vec2& size) noexcept {
    glBindTexture(GL_TEXTURE_2D, m_ssaoTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, static_cast<GLsizei>(size[0]), static_cast<GLsizei>(size[1]), 0, GL_RED, GL_FLOAT, nullptr);

    glBindTexture(GL_TEXTURE_2D, m_ssaoBlurTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, static_cast<GLsizei>(size[0]), static_cast<GLsizei>(size[1]), 0, GL_RED, GL_FLOAT, nullptr);

    m_size = glm::vec2(size[0], size[1]);
}

void SSAOPass::bindMainPass(uint32_t positionTexture, uint32_t normalTexture, const glm::mat4& view, const glm::mat4& proj) const noexcept {
    m_ssaoPass->use();
    m_ssaoPass->setMat4("proj", proj);
    m_ssaoPass->setMat4("view", view);
    m_ssaoPass->setVec2("attachmentSize", m_size);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, positionTexture);
    m_ssaoPass->setInt("positionMap", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalTexture);
    m_ssaoPass->setInt("normalMap", 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_noiseTexture);
    m_ssaoPass->setInt("noiseMap", 2);

    for (unsigned int i = 0; i < 64; ++i)
        m_ssaoPass->setVec3("samples[" + std::to_string(i) + "]", m_ssaoKernel[i]);

    glViewport(0, 0, static_cast<GLsizei>(m_size[0]), static_cast<GLsizei>(m_size[1]));
    glBindFramebuffer(GL_FRAMEBUFFER, m_ssaoFBO);
    glClear(GL_COLOR_BUFFER_BIT);
}

void SSAOPass::bindBlurPass() const noexcept {
    m_blurPass->use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_ssaoTexture);
    m_blurPass->setInt("ssaoMap", 0);

    glViewport(0, 0, static_cast<GLsizei>(m_size[0]), static_cast<GLsizei>(m_size[1]));
    glBindFramebuffer(GL_FRAMEBUFFER, m_ssaoBlurFBO);
    glClear(GL_COLOR_BUFFER_BIT);
}
