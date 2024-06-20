/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** GBufferPass
*/

#pragma once

#include "Renderer/ShaderProgram.hpp"
#include "Renderer/Window.hpp"

#include <memory>

class GBufferPass {
    public:
        GBufferPass(std::shared_ptr<Window>& window);
        ~GBufferPass();

        GBufferPass(const GBufferPass&) = delete;
        GBufferPass& operator=(const GBufferPass&) = delete;

        GBufferPass(GBufferPass&&) = default;
        GBufferPass& operator=(GBufferPass&&) = delete;

        void bindFramebuffer() const noexcept;
        void bindStaticShader(const glm::mat4& view, const glm::mat4& proj) const noexcept;
        void bindSkinnedShader(const glm::mat4& view, const glm::mat4& proj) const noexcept;

        void resize(const glm::vec2& size) const;

        [[nodiscard]] uint32_t getPositionTexture() const noexcept { return m_positionTexture; }
        [[nodiscard]] uint32_t getNormalTexture() const noexcept { return m_normalTexture; }
        [[nodiscard]] uint32_t getAlbedoTexture() const noexcept { return m_albedoTexture; }
        [[nodiscard]] uint32_t getPbrTexture() const noexcept { return m_pbrTexture; }
        [[nodiscard]] uint32_t getDepthTexture() const noexcept { return m_depthTexture; }
        [[nodiscard]] std::shared_ptr<ShaderProgram> getStaticShaderProgram() const noexcept { return m_staticShaderProgram; }
        [[nodiscard]] std::shared_ptr<ShaderProgram> getSkinnedShaderProgram() const noexcept { return m_skinnedShaderProgram; }

    private:
        std::shared_ptr<Window> m_window;
        std::shared_ptr<ShaderProgram> m_staticShaderProgram;
        std::shared_ptr<ShaderProgram> m_skinnedShaderProgram;

        uint32_t m_gBuffer = 0;
        uint32_t m_positionTexture = 0;
        uint32_t m_normalTexture = 0;
        uint32_t m_albedoTexture = 0;
        uint32_t m_pbrTexture = 0;
        uint32_t m_depthTexture = 0;

        uint32_t m_quadVAO = 0;
        uint32_t m_quadVBO = 0;
};
