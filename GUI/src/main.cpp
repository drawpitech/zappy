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
        if (argc != 5 or std::string(argv[1]) != "-p" or std::stoi(argv[2]) <= 0)
            throw std::runtime_error("Usage: ./zappy_gui -p [port] -h [host]");

        App app(std::stoi(argv[2]), argv[4]);
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 84;
    }

    return 0;
}
