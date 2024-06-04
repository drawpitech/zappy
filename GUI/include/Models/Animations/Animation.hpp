/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** Animation
*/

#pragma once

#include "Models/SkeletalMeshImpl.hpp"
#include "Bone.hpp"

#include "assimp/anim.h"
#include "assimp/scene.h"

#include <vector>
#include <map>

struct AssimpNodeData {
    glm::mat4 transformation = glm::mat4(1.0);
    std::vector<AssimpNodeData> children;
    unsigned int childrenCount = 0;
    std::string name;
};

class Animation {
    public:
        Animation(const std::string& animationPath, const std::shared_ptr<SkeletalMeshImpl>& mesh);
        ~Animation() = default;

        Animation(const Animation&) = delete;
        Animation& operator=(const Animation&) = delete;

        Animation(Animation&&) = delete;
        Animation& operator=(Animation&&) = delete;

        [[nodiscard]] Bone* findBone(const std::string& name);

        [[nodiscard]] float getTicksPerSecond() const { return static_cast<float>(m_ticksPerSecond); }
        [[nodiscard]] float getDuration() const { return m_duration;}
        [[nodiscard]] const AssimpNodeData& getRootNode() const { return m_rootNode; }
        [[nodiscard]] const std::map<std::string, BoneInfo>& getBoneIDMap() const { return m_boneInfoMap; }

    private:
        void readMissingBones(const aiAnimation* animation, const std::shared_ptr<SkeletalMeshImpl>& mesh);
        void readHierarchyData(AssimpNodeData& dest, const aiNode* src);

        std::map<std::string, BoneInfo> m_boneInfoMap;
        std::vector<Bone> m_bones;
        AssimpNodeData m_rootNode;
        int m_ticksPerSecond;
        float m_duration;
};
