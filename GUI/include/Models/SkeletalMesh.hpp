/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** SkeletalMesh
*/

#pragma once

#include "Renderer/GlRenderer/ShaderProgram.hpp"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "glm/fwd.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <map>

struct BoneInfo {
    glm::mat4 offset;
    int id;
};

class SkeletalMesh {
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
            glm::ivec4 boneIds;
            glm::vec4 boneWeights;
        };

        struct Submesh {
            uint32_t vao;
            uint32_t vbo;
            uint32_t ibo;
            uint32_t indexCount;
            uint32_t materialIndex;
            glm::mat4 transform;
        };

    public:
        explicit SkeletalMesh(const std::string& modelPath);
        ~SkeletalMesh();

        SkeletalMesh(const SkeletalMesh&) = delete;
        SkeletalMesh& operator=(const SkeletalMesh&) = delete;

        SkeletalMesh(SkeletalMesh&&) = default;
        SkeletalMesh& operator=(SkeletalMesh&&) = delete;

        auto& GetBoneInfoMap() { return m_boneInfoMap; }
        int& GetBoneCount() { return m_boneCounter; }

        void draw(const std::shared_ptr<ShaderProgram>& shaderProgram, const glm::mat4& transform, const glm::vec3& color) const noexcept;

    private:
        std::map<std::string, BoneInfo> m_boneInfoMap;
        int m_boneCounter = 0;

        std::vector<Submesh> m_submeshes;
        std::vector<Material> m_materials;

        static void loadTexture(const std::string& texturePath, const std::string& modelPath, uint32_t& texture);
        void loadMaterials(const aiScene *scene, const std::string& modelPath);
        void processNode(const aiNode *node, const aiScene *scene);
        void processMesh(const aiMesh *mesh, const glm::mat4& transform);
};
