/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** SSRPass
*/

#pragma once

#include "Renderer/ShaderProgram.hpp"
#include "Renderer/Window.hpp"

#include <cstdint>
#include <memory>
#include <sys/types.h>

class SSRPass {
    public:
        SSRPass(std::shared_ptr<Window>& window);
        ~SSRPass();

        SSRPass(const SSRPass&) = delete;
        SSRPass& operator=(const SSRPass&) = delete;

        SSRPass(SSRPass&&) = default;
        SSRPass& operator=(SSRPass&&) = default;

        void bind(uint32_t gBufferNormal, uint32_t gBufferAlbedo, uint32_t gBufferDepth, const glm::mat4& proj, const glm::mat4& view) const noexcept;

        void resize(const glm::vec2& size) noexcept;

        [[nodiscard]] uint32_t getSSRTexture() const noexcept { return m_ssrTexture; }
        [[nodiscard]] uint32_t getFramebuffer() const noexcept { return m_ssrFBO; }

    private:
        std::shared_ptr<Window> m_window;
        std::unique_ptr<ShaderProgram> m_shaderProgram;

        glm::vec2 m_size;

        uint32_t m_ssrFBO = 0;
        uint32_t m_ssrTexture = 0;
};
