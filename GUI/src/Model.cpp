/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** Model
*/

#include "Model.hpp"

#include "ShaderProgram.hpp"
#include "assimp/Importer.hpp"
#include "assimp/matrix4x4.h"
#include "assimp/mesh.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"

Model::Model(const std::string& filePath) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(filePath,
        aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_GenNormals |
        aiProcess_ValidateDataStructure | aiProcess_GlobalScale
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        throw std::runtime_error("Failed to load model: " + std::string(importer.GetErrorString()));

    processNode(scene->mRootNode, scene);
}

Model::~Model() {
}

void Model::draw(std::shared_ptr<ShaderProgram> shaderProgram) const noexcept {
    for (const auto& submesh : m_submeshes) {
        shaderProgram->setMat4("model", submesh->transform);
        glBindVertexArray(submesh->vao);
        glDrawElements(GL_TRIANGLES, submesh->indexCount, GL_UNSIGNED_INT, nullptr);
    }
}

void Model::processNode(const aiNode *node, const aiScene *scene) noexcept {
    for (std::size_t i = 0; i < node->mNumMeshes; ++i) {
        const aiMatrix4x4 transform = node->mTransformation;
        const aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, glm::make_mat4(&transform.a1));
    }

    for (std::size_t i = 0; i < node->mNumChildren; ++i)
        processNode(node->mChildren[i], scene);
}

void Model::processMesh(const aiMesh *mesh, const glm::mat4& transform) noexcept {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex = {
            .position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z },
            .normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z },
            .texCoords = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y }
        };

        vertices.push_back(vertex);
    }

    for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
        const aiFace& face = mesh->mFaces[i];
        for (uint32_t j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    Submesh submesh = {
        .vao = 0,
        .vbo = 0,
        .ibo = 0,
        .indexCount = static_cast<uint32_t>(indices.size()),
        .transform = transform
    };

    glGenVertexArrays(1, &submesh.vao);
    glBindVertexArray(submesh.vao);

    glGenBuffers(1, &submesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, submesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &submesh.ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, submesh.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, position)));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, texCoords)));

    glBindVertexArray(0);
    m_submeshes.push_back(std::make_unique<Submesh>(submesh));
}
