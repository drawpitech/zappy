/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** App
*/

#pragma once

#include "Renderer/Renderer.hpp"

#include "glm/ext/vector_float2.hpp"

class App {
    private:
        enum RessourceType {
            FOOD = 0,
            LINEMATE = 1,
            DERAUMERE = 2,
            SIBUR = 3,
            MENDIANE = 4,
            PHIRAS = 5,
            THYSTAME = 6,
        };

        struct TileContent {
            std::array<int, 7> ressources = {0, 0, 0, 0, 0, 0, 0};
        };

    public:
        App(int port);
        ~App();

        App(const App&) = delete;
        App& operator=(const App&) = delete;

        App(App&&) = default;
        App& operator=(App&&) = default;

        void run();

    private:
        std::unique_ptr<Renderer> m_renderer;
        std::shared_ptr<Renderer::Scene> m_scene;

        glm::vec2 m_mapSize = {0, 0};
        unsigned int m_speed = 0;
        std::vector<std::vector<TileContent>> m_map;

        int m_socket = 0;
        void connectToServer(int port);

        static glm::vec2 getMapSize(const std::string& buffer);
        void updateMap(const std::string& buffer);

        void parseConnectionResponse();
        void createIslands();
};
