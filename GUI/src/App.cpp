/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** App
*/

#include "App.hpp"
#include "ShaderProgram.hpp"
#include "Window.hpp"

#include <chrono>
#include <memory>
#include <iostream>

App::App() {
    m_window = std::make_shared<Window>(1280, 720, "Zappy");
    m_camera = std::make_shared<Camera>(m_window);
    m_camera->setPerspective(45, static_cast<float>(m_window->getWidth()) / static_cast<float>(m_window->getHeight()), 0.1f, 100.0f);
    m_shaderProgram = std::make_shared<ShaderProgram>("../GUI/shaders/vertex.glsl", "../GUI/shaders/fragment.glsl");
}

App::~App() {
}

void App::handleUserInput() noexcept {
    m_window->pollEvents();

    if (glfwGetMouseButton(m_window->getHandle(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        m_window->showCursor(false);
        m_camera->disableCursorCallback(false);
    } else {
        m_camera->disableCursorCallback(true);
        m_window->showCursor(true);
        m_camera->resetMousePosition();
    }

    m_camera->update(m_deltaTime);
}

void App::updateDeltaTime() noexcept {
    m_frameEndTime = std::chrono::high_resolution_clock::now();
    m_deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(m_frameEndTime - m_frameStartTime).count();
    m_frameStartTime = m_frameEndTime;

    std::cout << "FPS: " << 1.0f / m_deltaTime << "\r" << std::flush;
}

void App::run() {
    // Vertex array creation
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Vertex buffer creation
    GLfloat vertices[] = {
        // front
        -0.5, -0.5, 0.5,
        0.5, -0.5, 0.5,
        0.5, 0.5, 0.5,
        -0.5, 0.5, 0.5,
        // back
        -0.5, -0.5, -0.5,
        0.5, -0.5, -0.5,
        0.5, 0.5, -0.5,
        -0.5, 0.5, -0.5
    };

    GLushort indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};

    unsigned int VBO, EBO;

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while (!m_window->shouldClose()) {
        handleUserInput();
        updateDeltaTime();

        m_window->clear();
        m_shaderProgram->use();
        m_shaderProgram->setMat4("view", m_camera->getViewMatrix());
        m_shaderProgram->setMat4("proj", m_camera->getProjectionMatrix());

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);
        glUseProgram(0);

        m_window->swapBuffers();
    }
}
