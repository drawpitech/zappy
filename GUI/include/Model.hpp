/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** Model
*/

#pragma once

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "glm/glm.hpp"

#include <string>
#include <vector>
#include <memory>

class Model {
    public:
        explicit Model(const std::string& filePath);
        ~Model();

        Model(const Model&) = delete;
        Model& operator=(const Model&) = delete;

        void draw() const noexcept;

    private:
        struct Vertex {
            glm::vec3 position;
            glm::vec3 normal;
            glm::vec2 texCoords;
        };

        struct Submesh {
            uint32_t vao;
            uint32_t vbo;
            uint32_t ibo;
            uint32_t indexCount;
        };

    private:
        std::vector<std::unique_ptr<Submesh>> m_submeshes;

        void processNode(const aiNode *node, const aiScene *scene) noexcept;
        void processMesh(const aiMesh *mesh) noexcept;
};
