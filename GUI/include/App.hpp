/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** App
*/

#pragma once

#include "Models/Animations/Animator.hpp"
#include "Models/SkeletalMesh.hpp"
#include "Renderer/Renderer.hpp"

#include "glm/ext/vector_float2.hpp"
#include "imgui.h"
#include <memory>

#define GREEN ImVec4(0, 1, 0, 1)
#define RED ImVec4(1, 0, 0, 1)
#define BLUE ImVec4(0, 0, 1, 1)
#define WHITE ImVec4(1, 1, 1, 1)
#define LOG(message, color) m_logs.emplace_back(message, color)

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
            int orientation;
            std::string teamName;
            int level;
            std::shared_ptr<Animator> animator;
        };

        struct Team {
            std::shared_ptr<SkeletalMesh> mesh;
        };

        class LogMessage {
            public:
                LogMessage(std::string message, ImVec4 color)
                    : m_message(std::move(message)), m_color(color), m_time(time(nullptr)) {}

                [[nodiscard]] const std::string& getMessage() const { return m_message; }
                [[nodiscard]] const ImVec4& getColor() const { return m_color; }
                [[nodiscard]] const time_t &getTime() const { return m_time; }

            private:
                std::string m_message;
                ImVec4 m_color;
                time_t m_time;
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

        std::unordered_map<std::string, Team> m_teams;
        std::map<int, Player> m_players;

        // Dict of all the ressources meshes and offsets on the tiles
        std::map<RessourceType, glm::vec3> m_ressourceOffset;
        std::map<RessourceType, const std::shared_ptr<StaticMesh>> m_ressourceMesh;
        // Dict of all the player meshes and animations
        
        std::map<std::string, std::shared_ptr<SkeletalMesh>> m_playerMeshes;
        std::map<std::string, std::shared_ptr<Animation>> m_playerAnims;
        std::shared_ptr<StaticMesh> m_islandMesh;

        std::vector<LogMessage> m_logs;

        int m_socket = 0;
        void connectToServer(int port);

        void updateMap(const std::string& bufferView);
        void updatePlayers(const std::string& bufferView);
        static glm::ivec2 parseMapSize(const std::string& bufferView);
        void parseConnectionResponse();

        void createScene();
        void createRessources();
};
