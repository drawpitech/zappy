/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** Animation
*/

#include "Models/Animations/Animation.hpp"

#include "Utils.hpp"

#include "assimp/anim.h"
#include "assimp/postprocess.h"

Animation::Animation(const std::string& animationPath, const std::shared_ptr<SkeletalMesh>& mesh) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
    if ((scene == nullptr) || ((scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) != 0) || (scene->mRootNode == nullptr))
        throw std::runtime_error("Failed to load animation file: " + animationPath + " " + importer.GetErrorString());
    if (scene->mAnimations == nullptr)
        throw std::runtime_error("No animations found in file: " + animationPath);

    aiAnimation *animation = *scene->mAnimations;
    m_duration = static_cast<float>(animation->mDuration);
    m_ticksPerSecond = static_cast<int>(animation->mTicksPerSecond);

    readHierarchyData(m_rootNode, scene->mRootNode);
    readMissingBones(animation, mesh);
}

Bone* Animation::findBone(const std::string& name) {
    const auto& iter = std::find_if(m_bones.begin(), m_bones.end(), [&](const Bone& Bone) {
        return Bone.getBoneName() == name;
    });

    if (iter == m_bones.end())
        return nullptr;
    return &(*iter);
}

void Animation::readMissingBones(const aiAnimation* animation, const std::shared_ptr<SkeletalMesh>& mesh) {
    unsigned int size = animation->mNumChannels;

    auto& boneInfoMap = mesh->GetBoneInfoMap();
    int& boneCount = mesh->GetBoneCount();

    for (int i = 0; i < size; i++) {
        aiNodeAnim *channel = animation->mChannels[i];
        std::string boneName = channel->mNodeName.data;

        if (boneInfoMap.find(boneName) == boneInfoMap.end()) {
            boneInfoMap[boneName].id = boneCount;
            boneCount++;
        }

        m_bones.emplace_back(channel->mNodeName.data, boneInfoMap[channel->mNodeName.data].id, channel);
    }

    m_boneInfoMap = boneInfoMap;
}

void Animation::readHierarchyData(AssimpNodeData& dest, const aiNode* src) {
    if (src == nullptr)
        throw std::runtime_error("Node is null");

    dest.name = static_cast<const char*>(src->mName.data);
    dest.transformation = Utils::assimpToGlmMat4(src->mTransformation);
    dest.childrenCount = src->mNumChildren;

    for (int i = 0; i < src->mNumChildren; i++) {
        AssimpNodeData newData;
        readHierarchyData(newData, src->mChildren[i]);
        dest.children.push_back(newData);
    }
}
