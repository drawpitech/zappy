/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** LightingPass
*/

#pragma once

#include "ShaderProgram.hpp"
#include "Window.hpp"

#include <cstdint>
#include <memory>

class LightingPass {
    public:
        LightingPass(std::shared_ptr<Window> window);
        ~LightingPass();

        void bind(
            uint32_t positionTexture, uint32_t albedoTexture, uint32_t normalTexture, uint32_t pbrTexture, uint32_t ssaoTexture) const noexcept;
        void renderQuad() const noexcept;

    private:
        std::shared_ptr<Window> m_window;
        std::unique_ptr<ShaderProgram> m_shaderProgram;

        uint32_t m_quadVAO;
        uint32_t m_quadVBO;
};
