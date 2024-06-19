/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** Animator
*/

#include "Models/Animations/Animator.hpp"

#include "Models/Animations/Bone.hpp"

#include <utility>

Animator::Animator(std::shared_ptr<Animation> animation) : m_currentAnimation(std::move(animation)) {
}

void Animator::updateAnimation(float deltaTime) noexcept  {
    m_deltaTime = deltaTime;
    if (m_currentAnimation != nullptr) {
        m_currentTime += m_currentAnimation->getTicksPerSecond() * deltaTime;
        m_currentTime = std::fmod(m_currentTime, m_currentAnimation->getDuration());
        calculateBoneTransform(m_currentAnimation->getRootNode(), glm::mat4(1.0));
    }
}

void Animator::playAnimation(const std::shared_ptr<Animation>& animation) noexcept {
    m_currentAnimation = animation;
    m_currentTime = 0.0;
}

void Animator::calculateBoneTransform(const AssimpNodeData& node, glm::mat4 parentTransform) noexcept {
    std::string nodeName = node.name;
    glm::mat4 nodeTransform = node.transformation;

    Bone* bone = m_currentAnimation->findBone(nodeName);
    if (bone != nullptr) {
        bone->update(m_currentTime);
        nodeTransform = bone->getLocalTransform();
    }

    glm::mat4 globalTransformation = parentTransform * nodeTransform;

    auto boneInfoMap = m_currentAnimation->getBoneIDMap();
    if (boneInfoMap.find(nodeName) != boneInfoMap.end()) {
        int index = boneInfoMap[nodeName].id;
        glm::mat4 offset = boneInfoMap[nodeName].offset;
        m_finalBoneMatrices.at(index) = globalTransformation * offset;
    }

    for (int i = 0; i < node.childrenCount; i++)
        calculateBoneTransform(node.children[i], globalTransformation);
}

std::array<glm::mat4, MAX_BONES> Animator::getFinalBoneMatrices() const noexcept {
    return m_finalBoneMatrices;
}
