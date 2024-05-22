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
        App app;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 84;
    }

    return 0;
}
