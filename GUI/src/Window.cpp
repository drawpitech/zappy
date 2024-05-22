/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** Window
*/

#include "Window.hpp"

#include "GLFW/glfw3.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <stdexcept>
#include <iostream>

#define DEBUG 1

static void framebufferResizeCallback(GLFWwindow* window, int width, int height) noexcept {
    Window *self = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
    self->wasResized = true;
    self->resize(width, height);
}

static void APIENTRY debugCallback(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char *message, const void *userParam) {
    (void) userParam, (void) length;

    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::string debugMessage;
    debugMessage += "\n============================================================\n";
    debugMessage += "Debug message (" + std::to_string(id) + "): " + message + "\n";

    switch (source) {
        case GL_DEBUG_SOURCE_API:             debugMessage += "- Source: API\n"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   debugMessage += "- Source: Window System\n"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: debugMessage += "- Source: Shader Compiler\n"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     debugMessage += "- Source: Third Party\n"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     debugMessage += "- Source: Application\n"; break;
        case GL_DEBUG_SOURCE_OTHER:           debugMessage += "- Source: Other\n"; break;
    };

    switch (type) {
        case GL_DEBUG_TYPE_ERROR:               debugMessage += "- Type: Error\n"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: debugMessage += "- Type: Deprecated Behaviour\n"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  debugMessage += "- Type: Undefined Behaviour\n"; break;
        case GL_DEBUG_TYPE_PORTABILITY:         debugMessage += "- Type: Portability\n"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         debugMessage += "- Type: Performance\n"; break;
        case GL_DEBUG_TYPE_MARKER:              debugMessage += "- Type: Marker\n"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          debugMessage += "- Type: Push Group\n"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           debugMessage += "- Type: Pop Group\n"; break;
        case GL_DEBUG_TYPE_OTHER:               debugMessage += "- Type: Other\n"; break;
    };

    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:         debugMessage += "- Severity: high\n"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       debugMessage += "- Severity: medium\n"; break;
        case GL_DEBUG_SEVERITY_LOW:          debugMessage += "- Severity: low\n"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: debugMessage += "- Severity: notification\n"; break;
    };
    debugMessage += "============================================================\n";

    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            throw std::runtime_error(debugMessage);
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            std::clog << "Medium error: " << debugMessage << std::endl;
            break;
        case GL_DEBUG_SEVERITY_LOW:
            std::clog << "Low error: " << debugMessage << std::endl;
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            std::clog << "Notification: " << debugMessage << std::endl;
            break;
    }
}

Window::Window(int width, int height, const std::string& title) : m_width(width), m_height(height) {
    if (glfwInit() == GLFW_FALSE)
        throw std::runtime_error("Failed to initialize GLFW");
    m_deletionQueue.add([this]() {
        glfwTerminate();
    });

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #if DEBUG
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    #endif

    m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (m_window == nullptr)
        throw std::runtime_error("Failed to create GLFW window");
    m_deletionQueue.add([this]() {
        glfwDestroyWindow(m_window);
    });
    glfwMakeContextCurrent(m_window);

    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, framebufferResizeCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw std::runtime_error("Failed to initialize GLAD");

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glViewport(0, 0, width, height);

    #if DEBUG
        int flags;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(debugCallback, nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        }
    #endif


    {   // ImGui initialization
        IMGUI_CHECKVERSION();
        if (!ImGui::CreateContext() || !ImGui_ImplGlfw_InitForOpenGL(m_window, true) || !ImGui_ImplOpenGL3_Init("#version 460"))
            throw std::runtime_error("Failed to initialize ImGui");

        ImGui::StyleColorsDark();

        m_deletionQueue.add([]() {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
        });
    }
}

Window::~Window() {
    m_deletionQueue.flush();
}

void Window::resize(int width, int height) noexcept {
    m_width = width;
    m_height = height;
    glViewport(0, 0, width, height);
}
