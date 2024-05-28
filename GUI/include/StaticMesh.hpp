/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** StaticMesh
*/

#pragma once

#include "ShaderProgram.hpp"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"

#include <cstdint>
#include <memory>
#include <memory>
#include <string>
#include <vector>

class StaticMesh {
    public:
        explicit StaticMesh(const std::string& modelPath);
        ~StaticMesh();

        StaticMesh(const StaticMesh&) = delete;
        StaticMesh& operator=(const StaticMesh&) = delete;

        StaticMesh(StaticMesh&&) = default;
        StaticMesh& operator=(StaticMesh&&) = delete;

        void draw(const std::shared_ptr<ShaderProgram>& shaderProgram, const glm::mat4& transform) const noexcept;

    private:
        struct Material {
            uint32_t albedoMap;
            uint32_t metallicRoughnessMap;
            uint32_t normalMap;
        };

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
            uint32_t materialIndex;
            glm::mat4 transform;
        };

        std::vector<Submesh> m_submeshes;
        std::vector<Material> m_materials;

        void loadMaterials(const aiScene *scene, const std::string& modelPath);
        static void loadTexture(const std::string& texturePath, const std::string& modelPath, uint32_t& texture);
        void processNode(const aiNode *node, const aiScene *scene);
        void processMesh(const aiMesh *mesh, const glm::mat4& transform);
};
