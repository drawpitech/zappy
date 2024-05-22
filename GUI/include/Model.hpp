/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** Model
*/

#pragma once

#include "ShaderProgram.hpp"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "glm/glm.hpp"

#include <cstdint>
#include <string>
#include <vector>
#include <memory>

class Model {
    public:
        explicit Model(const std::string& modelPath);
        ~Model();

        Model(const Model&) = delete;
        Model& operator=(const Model&) = delete;

        void draw(std::shared_ptr<ShaderProgram> shaderProgram) const noexcept;

    private:
        struct Material {
            uint32_t albedoMap;
            uint32_t metallicRoughnessMap;
            uint32_t normalMap;
            uint32_t aoMap;
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

    private:
        std::vector<Submesh> m_submeshes;
        std::vector<Material> m_materials;

        void loadMaterials(const aiScene *scene, const std::string& modelPath);
        void loadTexture(const std::string& texturePath, const std::string& modelPath, uint32_t& texture);
        void processNode(const aiNode *node, const aiScene *scene);
        void processMesh(const aiMesh *mesh, const glm::mat4& transform);
};
