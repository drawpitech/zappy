/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** ShaderProgram
*/

#include "ShaderProgram.hpp"

#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

ShaderProgram::ShaderProgram(const std::string &vertexShaderPath, const std::string &fragmentShaderPath) {
    programID = glCreateProgram();

    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    loadShader(vertexShaderPath, vertexShaderID);

    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    loadShader(fragmentShaderPath, fragmentShaderID);

    glLinkProgram(programID);
    checkProgram();

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(programID);
}

void ShaderProgram::use() noexcept {
    glUseProgram(programID);
}

void ShaderProgram::loadShader(const std::string &shaderPath, GLuint shaderID) {
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

    glAttachShader(programID, shaderID);
}

void ShaderProgram::checkShader(GLuint shaderID) {
    int success = 0;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
        throw std::runtime_error("Failed to compile shader: " + std::string(infoLog));
    }
}

void ShaderProgram::checkProgram() {
    int success = 0;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(programID, 512, NULL, infoLog);
        throw std::runtime_error("Failed to link shader program: " + std::string(infoLog));
    }
}

void ShaderProgram::setMat4(const std::string &name, const glm::mat4 &value) noexcept {
    glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}
