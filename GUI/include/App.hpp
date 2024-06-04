/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** App
*/

#pragma once

#include "Renderer/Renderer.hpp"

class App {
    public:
        App();
        ~App();

        App(const App&) = delete;
        App& operator=(const App&) = delete;

        App(App&&) = default;
        App& operator=(App&&) = default;

        void run();

    private:
        std::unique_ptr<Renderer> m_renderer;
};
