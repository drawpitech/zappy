/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** LightingPass
*/

#pragma once

#include "Renderer/GlRenderer/ShaderProgram.hpp"
#include "Renderer/GlRenderer/Window.hpp"

#include "glm/ext/vector_float3.hpp"
#include "imgui.h"

#include <cstdint>
#include <memory>

class LightingPass {
    public:
        LightingPass(std::shared_ptr<Window>& window);
        ~LightingPass();

        LightingPass(const LightingPass&) = delete;
        LightingPass& operator=(const LightingPass&) = delete;

        LightingPass(LightingPass&&) = default;
        LightingPass& operator=(LightingPass&&) = default;

        void resize(const glm::vec2& size) noexcept;
        void bind(uint32_t positionTexture, uint32_t albedoTexture, uint32_t normalTexture, uint32_t pbrTexture, uint32_t ssaoTexture, uint32_t ssrTexture, const glm::vec3& camPos, const glm::mat4& view, const glm::mat4& proj, int debugView) noexcept;

        [[nodiscard]] glm::vec2 getViewportSize() const noexcept { return {m_lightingPassSize.x, m_lightingPassSize.y}; }
        bool wasResized = false;    // NOLINT

    private:
        std::shared_ptr<Window> m_window;
        std::unique_ptr<ShaderProgram> m_lightingProgram;
        std::unique_ptr<ShaderProgram> m_equiRectangularToCubemapProgram;
        std::unique_ptr<ShaderProgram> m_irradianceProgram;
        std::unique_ptr<ShaderProgram> m_backgroundProgram;

        ImVec2 m_lightingPassSize = {100, 100};

        unsigned int m_framebuffer = 0;
        unsigned int m_renderbuffer = 0;

        unsigned int m_captureFBO = 0;
        unsigned int m_captureRBO = 0;

        unsigned int m_hdrTexture = 0;
        unsigned int m_envCubemap = 0;
        unsigned int m_irradianceMap = 0;
};
