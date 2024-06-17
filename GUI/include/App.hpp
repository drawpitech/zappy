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

        struct Player {
            glm::vec3 position;
            glm::vec2 direction;
            std::string team;
            int level;
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

        glm::ivec2 m_mapSize = {0, 0};
        unsigned int m_speed = 0;
        int m_ressourceLayer = 4;
        std::vector<std::vector<TileContent>> m_map;
        glm::vec3 m_tileSize = {4, 4, 4};
        glm::vec2 m_tileSpacing = {0.5, 0.5};
        float m_tileHeight = 0;
        float m_playerHeight = 0;
        float m_resourceHeight = 0;
        glm::vec3 m_resourceSize = {0.5, 0.5, 0.5};
        std::map<int, Player> m_players;

        int m_socket = 0;
        void connectToServer(int port);

        void updateMap(const std::string& bufferView);
        void updatePlayers(const std::string& bufferView);
        static glm::ivec2 parseMapSize(const std::string& bufferView);
        void parseConnectionResponse();

        void createScene();
        void createIslands();
        void createRessources();
};
