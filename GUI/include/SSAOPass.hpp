/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** SSAOPass
*/

#pragma once

#include "ShaderProgram.hpp"
#include "Window.hpp"

#include <cstdint>
#include <memory>
#include <sys/types.h>

class SSAOPass {
    public:
        SSAOPass(std::shared_ptr<Window> window);
        ~SSAOPass();

        void bindMainPass(uint32_t positionTexture, uint32_t normalTexture, const glm::mat4& view, const glm::mat4& proj) const noexcept;
        void bindBlurPass() const noexcept;
        void renderQuad() const noexcept;

        [[nodiscard]] uint32_t getSSAOTexture() const noexcept { return m_ssaoTexture; }
        [[nodiscard]] uint32_t getSSAOBlurTexture() const noexcept { return m_ssaoBlurTexture; }
        [[nodiscard]] uint32_t getFramebuffer() const noexcept { return m_ssaoFBO; }
        [[nodiscard]] uint32_t getBluredFramebuffer() const noexcept { return m_ssaoBlurFBO; }

    private:
        std::shared_ptr<Window> m_window;
        std::unique_ptr<ShaderProgram> m_ssaoPass;
        std::unique_ptr<ShaderProgram> m_blurPass;

        std::vector<glm::vec3> m_ssaoKernel;
        uint32_t m_noiseTexture;

        uint32_t m_ssaoFBO;
        uint32_t m_ssaoTexture;
        uint32_t m_ssaoBlurFBO;
        uint32_t m_ssaoBlurTexture;

        uint32_t m_quadVAO;
        uint32_t m_quadVBO;
};
