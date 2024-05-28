/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** Utils
*/

#pragma once

#include <deque>
#include <functional>

class DeletionQueue {
    public:
        void add(std::function<void()>&& function);
        void flush();

    private:
        std::deque<std::function<void()>> deletors;
};
