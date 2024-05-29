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

        ShaderProgram(ShaderProgram&&) = default;
        ShaderProgram& operator=(ShaderProgram&&) = delete;

        void use() const noexcept;

        void setMat4(const std::string &name, const glm::mat4 &value) const noexcept;
        void setBool(const std::string &name, bool value) const noexcept;
        void setInt(const std::string &name, int value) const noexcept;
        void setVec3(const std::string &name, glm::vec3 value) const noexcept;

    private:
        void loadShader(const std::string &shaderPath, GLuint shaderID) const;
        static void checkShader(GLuint shaderID);
        void checkProgram() const;

        GLuint m_program;
};
