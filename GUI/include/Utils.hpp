/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** Utils
*/

#pragma once

#include <functional>
#include <deque>

struct DeletionQueue {
    std::deque<std::function<void()>> deletors;

    void add(std::function<void()>&& function);
    void flush();
};
