/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** Window
*/

#pragma once

#include "Utils.hpp"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <string>

class Window {
    public:
        explicit Window(int width, int height, const std::string& title);
        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        Window(Window&&)  noexcept = default;
        Window& operator=(Window&&) = delete;

        bool wasResized = false;    // NOLINT

        /**
         * @brief Check if the user has attempted to close the window
         *
         * @return true if the user has attempted to close the window
         * @return false otherwise
         */
        [[nodiscard]] bool shouldClose() const noexcept { return static_cast<bool>(glfwWindowShouldClose(m_window)); }

        /**
         * @brief I don't know what this does
         */
        void close() const noexcept { glfwSetWindowShouldClose(m_window, static_cast<int>(true)); }

        /**
         * @brief Poll for events such as key presses, mouse movement,
         * window resizing, etc.
         */
        static void pollEvents() noexcept { glfwPollEvents(); }

        /**
         * @brief Clear the window's color and depth buffers.
         */
        static void clear() noexcept { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

        /**
         * @brief Display the rendered image to the window.
         */
        void swapBuffers() const noexcept { glfwSwapBuffers(m_window); }

        /**
         * @brief Resize the window to the given width and height
         *
         * @param width the new width of the window
         * @param height the new height of the window
         */
        void resize(int width, int height) noexcept;

        /**
         * @brief Check if a key is currently pressed
         * @param keycode the key to check (GLFW_KEY_...)
         */
        [[nodiscard]] bool isKeyPressed(const int keycode) const noexcept { return glfwGetKey(m_window, keycode) == GLFW_PRESS; }

        void showCursor(const bool show) const noexcept { glfwSetInputMode(m_window, GLFW_CURSOR, show ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED); }

        /* Getters */
        [[nodiscard]] unsigned int getWidth() const noexcept { return m_width; }
        [[nodiscard]] unsigned int getHeight() const noexcept { return m_height; }
        [[nodiscard]] GLFWwindow* getHandle() const noexcept { return m_window; }

    private:
        GLFWwindow* m_window;
        int m_width = 0;
        int m_height = 0;

        DeletionQueue m_deletionQueue;
};
