/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** Utils
*/

#pragma once

#include <deque>
#include <functional>

#include "glm/glm.hpp"
#include "assimp/matrix4x4.h"

namespace Utils {
    class DeletionQueue {
        public:
            void add(std::function<void()>&& function);
            void flush();

        private:
            std::deque<std::function<void()>> deletors;
    };

    static inline glm::mat4 assimpToGlmMat4(const aiMatrix4x4& from) {
        return {
            from.a1, from.b1, from.c1, from.d1,
            from.a2, from.b2, from.c2, from.d2,
            from.a3, from.b3, from.c3, from.d3,
            from.a4, from.b4, from.c4, from.d4
        };
    }

    void renderQuad();
    void renderCube();
} // namespace Utils
