/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** Utils
*/

#include "Utils.hpp"

#include <ranges>

void DeletionQueue::add(std::function<void()>&& function) {
    deletors.push_back(std::move(function));
}

void DeletionQueue::flush() {
    for (auto & deletor : std::ranges::reverse_view(deletors))
        deletor();
    deletors.clear();
}
