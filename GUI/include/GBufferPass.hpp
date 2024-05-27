/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** GBufferPass
*/

#pragma once

#include "ShaderProgram.hpp"
#include "Window.hpp"

#include <memory>

class GBufferPass {
    public:
        GBufferPass(std::shared_ptr<Window> window);
        ~GBufferPass();

        void bind(const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj) const noexcept;
        void renderQuad() const noexcept;

        [[nodiscard]] uint32_t getPositionTexture() const noexcept { return m_positionTexture; }
        [[nodiscard]] uint32_t getNormalTexture() const noexcept { return m_normalTexture; }
        [[nodiscard]] uint32_t getAlbedoTexture() const noexcept { return m_albedoTexture; }
        [[nodiscard]] uint32_t getPbrTexture() const noexcept { return m_pbrTexture; }
        [[nodiscard]] uint32_t getDepthTexture() const noexcept { return m_depthTexture; }
        [[nodiscard]] std::shared_ptr<ShaderProgram> getShaderProgram() const noexcept { return m_shaderProgram; }

    private:
        std::shared_ptr<Window> m_window;
        std::shared_ptr<ShaderProgram> m_shaderProgram;

        uint32_t m_gBuffer;
        uint32_t m_positionTexture;
        uint32_t m_normalTexture;
        uint32_t m_albedoTexture;
        uint32_t m_pbrTexture;
        uint32_t m_depthTexture;
        uint32_t m_rbo;

        uint32_t m_quadVAO;
        uint32_t m_quadVBO;
};
