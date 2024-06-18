/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** main
*/

#include <iostream>

#include "App.hpp"

int main(int argc, char* argv[]) {
    (void) argc;
    (void) argv;

    try {
        if (argc != 2)
            throw std::runtime_error("Usage: ./zappy_gui [port]");

        App app(std::stoi(argv[1]));
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 84;
    }

    return 0;
}
