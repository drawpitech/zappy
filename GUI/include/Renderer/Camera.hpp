/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** Camera
*/

#pragma once

#include "Window.hpp"

#include <memory>

class Camera {
    public:
        explicit Camera(std::shared_ptr<Window>& window);
        ~Camera() = default;

        Camera(const Camera&) = delete;
        Camera& operator=(const Camera&) = delete;

        Camera(Camera&&) = default;
        Camera& operator=(Camera&&) = delete;

        /**
         * @brief Update the camera's position and view matrix
         * depending on the user's input.
         *
         * @param deltaTime The time between the last frame and the current frame.
         */
        void update(float deltaTime) noexcept;

        /**
         * @brief Set the Perspective matrix of the camera.
         *
         * @param fov The field of view.
         * @param aspect The aspect ratio.
         * @param near The near plane.
         * @param far The far plane.
         */
        void setPerspective(float fov, float aspect, float near, float far) noexcept;

        /**
         * @brief Reset the mouse position to the center of the
         * window to avoid sudden camera movements.
         * This function should be used after showing the cursor if it was hidden.
         */
        static void resetMousePosition() noexcept;

        /**
         * @brief Disable the cursor callback to avoid camera movement
         * based on mouse input.
         *
         * @param lock If true, the cursor will be locked in the center of the window.
         */
        static void disableCursorCallback(bool lock) noexcept;

        /* Getters */
        [[nodiscard]] const glm::mat4& getViewMatrix() const noexcept { return m_viewMatrix; }
        [[nodiscard]] const glm::mat4& getProjectionMatrix() const noexcept { return m_projectionMatrix; };
        [[nodiscard]] const glm::vec3& getPosition() const noexcept { return m_position; }

    private:
        std::shared_ptr<Window> m_window;

        glm::vec3 m_position {0, 0, 0};
        glm::vec3 m_upVector {0, 1, 0};

        glm::mat4 m_viewMatrix {1};
        glm::mat4 m_projectionMatrix {1};
};
