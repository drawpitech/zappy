/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** Utils
*/

#pragma once

#include <deque>
#include <functional>
#include <mutex>
#include <string>
#include <vector>

#include "glad/glad.h"

#include "glm/glm.hpp"
#include "assimp/matrix4x4.h"

namespace Utils {

    template<typename T, typename... Args>
    class Instance {
    public:
        Instance(const Instance&) = delete;
        Instance& operator=(const Instance&) = delete;
        Instance(Instance&&) = delete;
        Instance& operator=(Instance&&) = delete;

        static T* Get(Args&&... args) {
            static T inst{std::forward<Args>(args)...};
            return &inst;
        }

    private:
        Instance() = default;
        ~Instance() = default;
    };

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

    class ImageLoader {
    public:
        ImageLoader(const std::vector<std::string>& filepaths)
            : _filepaths(filepaths)
        { loadImages(); }
        ImageLoader(ImageLoader &&) = delete;
        ImageLoader(const ImageLoader &) = delete;
        ImageLoader &operator=(ImageLoader &&) = delete;
        ImageLoader &operator=(const ImageLoader &) = delete;
        ~ImageLoader() = default;

        void loadImages();
        [[nodiscard]] const std::vector<GLuint>& getImages() const {
            return _images;
        }

    private:
        void loadImage(const std::string& filepath);
        std::vector<std::string> _filepaths;
        std::vector<GLuint> _images;
        std::mutex _mutex;
    };
} // namespace Utils
