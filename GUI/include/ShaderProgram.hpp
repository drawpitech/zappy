/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** ShaderProgram
*/

#pragma once

#include "glad/glad.h"
#include "glm/glm.hpp"

#include <string>

class ShaderProgram {
    public:
        explicit ShaderProgram(const std::string &vertexShaderPath, const std::string &fragmentShaderPath);
        ~ShaderProgram();

        ShaderProgram(const ShaderProgram&) = delete;
        ShaderProgram& operator=(const ShaderProgram&) = delete;

        void use() noexcept;

        void setMat4(const std::string &name, const glm::mat4 &value) noexcept;

    private:
        void loadShader(const std::string &shaderPath, GLuint shaderID);
        void checkShader(GLuint shaderID);
        void checkProgram();

    private:
        GLuint m_program;
};
