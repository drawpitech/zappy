/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** Camera
*/

#include "Renderer/Camera.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "GLFW/glfw3.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/geometric.hpp"

// static members for mouse callback
static glm::vec3 front = glm::vec3(1, 0, 0); // NOLINT
static bool firstMouse = true; // NOLINT
static float lastX = 0; // NOLINT
static float lastY = 0; // NOLINT
static float yaw = 0; // NOLINT
static float pitch = 0; // NOLINT
static bool cursorLocked = false; // NOLINT
static float cameraSpeed = 4.f; // NOLINT

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    (void) window;
    if (cursorLocked)
        return;

    if (firstMouse) {
        lastX = static_cast<float>(xpos);
        lastY = static_cast<float>(ypos);
        firstMouse = false;
    }

    float xOffset = static_cast<float>(xpos) - lastX;
    float yOffset = lastY - static_cast<float>(ypos);
    lastX = static_cast<float>(xpos);
    lastY = static_cast<float>(ypos);

    float sensitivity = 0.1;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    yaw += xOffset;
    pitch -= yOffset;

    if (pitch > 89.0)
        pitch = 89.0;
    if (pitch < -89.0)
        pitch = -89.0;

    glm::vec3 direction = {
        cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
        sin(glm::radians(-pitch)),
        sin(glm::radians(yaw)) * cos(glm::radians(pitch))
    };

    front = glm::normalize(direction);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    (void) window;
    (void) xoffset;
    cameraSpeed += static_cast<float>(yoffset);
    if (cameraSpeed < 0.1)
        cameraSpeed = 0.1;
    if (cameraSpeed > 100)
        cameraSpeed = 100;
}

Camera::Camera(std::shared_ptr<Window>& window) : m_window(window) {
    glfwSetCursorPosCallback(m_window->getHandle(), mouseCallback);
    glfwSetScrollCallback(m_window->getHandle(), scrollCallback);
}

void Camera::disableCursorCallback(bool lock) noexcept {
    cursorLocked = lock;
}

void Camera::resetMousePosition() noexcept {
    firstMouse = true;
}

void Camera::setPerspective(float fov, float aspect, float near, float far) noexcept {
    m_projectionMatrix = glm::mat4(0);
    m_projectionMatrix = glm::perspective(glm::radians(fov), aspect, near, far);
}

void Camera::update(float deltaTime) noexcept {
    float offset = deltaTime * cameraSpeed;

    if (glfwGetKey(m_window->getHandle(), GLFW_KEY_W) == GLFW_PRESS)
        m_position += offset * front;
    if (glfwGetKey(m_window->getHandle(), GLFW_KEY_S) == GLFW_PRESS)
        m_position -= offset * front;
    if (glfwGetKey(m_window->getHandle(), GLFW_KEY_A) == GLFW_PRESS)
        m_position -= glm::normalize(glm::cross(front, m_upVector)) * offset;
    if (glfwGetKey(m_window->getHandle(), GLFW_KEY_D) == GLFW_PRESS)
        m_position += glm::normalize(glm::cross(front, m_upVector)) * offset;
    if (glfwGetKey(m_window->getHandle(), GLFW_KEY_SPACE) == GLFW_PRESS)
        m_position += offset * m_upVector;
    if (glfwGetKey(m_window->getHandle(), GLFW_KEY_C) == GLFW_PRESS)
        m_position -= offset * m_upVector;

    m_viewMatrix = glm::lookAt(m_position, m_position + front, m_upVector);
}
