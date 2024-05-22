/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** Utils
*/

#include "Utils.hpp"

void DeletionQueue::add(std::function<void()>&& function) {
    deletors.push_back(function);
}

void DeletionQueue::flush() {
    for (auto it = deletors.rbegin(); it != deletors.rend(); it++)
        (*it)();
    deletors.clear();
}
