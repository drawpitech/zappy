/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** ShaderProgram
*/

#include "Renderer/GlRenderer/ShaderProgram.hpp"

#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

ShaderProgram::ShaderProgram(const std::string &vertexShaderPath, const std::string &fragmentShaderPath) {
    m_program = glCreateProgram();  // NOLINT

    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    loadShader(vertexShaderPath, vertexShaderID);

    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    loadShader(fragmentShaderPath, fragmentShaderID);

    glLinkProgram(m_program);
    checkProgram();

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(m_program);
}

void ShaderProgram::use() const noexcept {
    glUseProgram(m_program);
}

void ShaderProgram::loadShader(const std::string &shaderPath, GLuint shaderID) const {
    std::ifstream shaderFile;
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    std::string shaderCode;

    try {
        shaderFile.open(shaderPath);
        std::stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
        shaderCode = shaderStream.str();
    } catch (std::ifstream::failure &e) {
        throw std::runtime_error("Failed to read shader file: " + shaderPath + ": " + e.what());
    }

    const char *shaderCodePtr = shaderCode.c_str();
    glShaderSource(shaderID, 1, &shaderCodePtr, NULL);
    glCompileShader(shaderID);
    checkShader(shaderID);

    glAttachShader(m_program, shaderID);
}

void ShaderProgram::checkShader(GLuint shaderID) {
    int success = 0;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (success == 0) {
        std::array<char, 512> infoLog{};
        glGetShaderInfoLog(shaderID, 512, nullptr, infoLog.data());
        throw std::runtime_error("Failed to compile shader: " + std::string(infoLog.data()));
    }
}

void ShaderProgram::checkProgram() const {
    int success = 0;
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if (success == 0) {
        std::array<char, 512> infoLog{};
        glGetProgramInfoLog(m_program, 512, NULL, infoLog.data());
        throw std::runtime_error("Failed to link shader program: " + std::string(infoLog.data()));
    }
}

void ShaderProgram::setMat4(const std::string &name, const glm::mat4 &value) const noexcept {
    glUniformMatrix4fv(glGetUniformLocation(m_program, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void ShaderProgram::setBool(const std::string &name, bool value) const noexcept {
    glUniform1i(glGetUniformLocation(m_program, name.c_str()), static_cast<int>(value));
}

void ShaderProgram::setInt(const std::string &name, int value) const noexcept {
    glUniform1i(glGetUniformLocation(m_program, name.c_str()), value);
}

void ShaderProgram::setVec3(const std::string &name, glm::vec3 value) const noexcept {
    glUniform3fv(glGetUniformLocation(m_program, name.c_str()), 1, glm::value_ptr(value));
}

void ShaderProgram::setVec2(const std::string &name, glm::vec2 value) const noexcept {
    glUniform2fv(glGetUniformLocation(m_program, name.c_str()), 1, glm::value_ptr(value));
}
