/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** SSAOPass
*/

#pragma once

#include "Renderer/GlRenderer/ShaderProgram.hpp"
#include "Renderer/GlRenderer/Window.hpp"

#include <cstdint>
#include <memory>
#include <sys/types.h>

class SSAOPass {
    public:
        SSAOPass(std::shared_ptr<Window>& window);
        ~SSAOPass();

        SSAOPass(const SSAOPass&) = delete;
        SSAOPass& operator=(const SSAOPass&) = delete;

        SSAOPass(SSAOPass&&) = default;
        SSAOPass& operator=(SSAOPass&&) = default;

        void bindMainPass(uint32_t positionTexture, uint32_t normalTexture, const glm::mat4& view, const glm::mat4& proj) const noexcept;
        void bindBlurPass() const noexcept;

        void resize(const glm::vec2& size) noexcept;

        [[nodiscard]] uint32_t getSSAOTexture() const noexcept { return m_ssaoTexture; }
        [[nodiscard]] uint32_t getSSAOBlurTexture() const noexcept { return m_ssaoBlurTexture; }
        [[nodiscard]] uint32_t getFramebuffer() const noexcept { return m_ssaoFBO; }
        [[nodiscard]] uint32_t getBluredFramebuffer() const noexcept { return m_ssaoBlurFBO; }

    private:
        std::shared_ptr<Window> m_window;
        std::unique_ptr<ShaderProgram> m_ssaoPass;
        std::unique_ptr<ShaderProgram> m_blurPass;

        glm::vec2 m_size;

        std::vector<glm::vec3> m_ssaoKernel;

        uint32_t m_noiseTexture = 0;
        uint32_t m_ssaoFBO = 0;
        uint32_t m_ssaoTexture = 0;
        uint32_t m_ssaoBlurFBO = 0;
        uint32_t m_ssaoBlurTexture = 0;
};
