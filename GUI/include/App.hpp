/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** App
*/

#pragma once

#include "Models/StaticMesh.hpp"
#include "Renderer/Renderer.hpp"
#include <memory>
#include <vector>

#define BUFFER_SIZE 1024000

#define LOG(message, color) m_logs.emplace_back(message, color)
#define GREEN ImVec4(0, 1, 0, 1)
#define RED ImVec4(1, 0, 0, 1)
#define BLUE ImVec4(0, 0, 1, 1)
#define WHITE ImVec4(1, 1, 1, 1)
#define YELLOW ImVec4(1, 1, 0, 1)

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

        enum AnimationType {
            DEFAULT = -1,
            IDLE = 0,
            MOVE = 1,
            EAT = 2,
            COLLECT = 3,
            RITUAL = 4,
            RITUALFAILURE = 5,
            RITUALSUCCESS = 6,
            BROADCAST = 7,
            EJECT = 8,
            BIRTH = 9
        };

        static constexpr std::size_t RESNUMBER = 7;

        struct TileContent {
            std::array<int, RESNUMBER> ressources = {0, 0, 0, 0, 0, 0, 0};
        };

        struct Tile {
            glm::vec3 position;
            std::shared_ptr<StaticMesh> mesh;
        };

        using inventory = TileContent;

        struct Player {
            glm::vec3 position;
            glm::vec3 visualPositionOffset = {0, 0, 0};
            int orientation;
            std::string teamName;
            int level;
            inventory inv{};
            AnimationType currentAnim = IDLE;
            AnimationType currentAction = IDLE;
            glm::vec3 moveOrientation = {0, 0, 0};
            std::chrono::time_point<std::chrono::steady_clock> animStartTime;
            std::shared_ptr<Animator> animator;
        };

        struct Egg {
            glm::vec3 position;
        };

        struct Team {
            std::pair<std::string, std::shared_ptr<SkeletalMesh>> mesh;
        };

        struct Broadcast {
            std::chrono::high_resolution_clock::time_point startTime;
            int playerID;
            glm::vec3 position;
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
        glm::vec3 m_tileSize = {2, 2, 2};
        glm::vec2 m_tileSpacing = {0, 0};
        float m_tileHeight = -2;
        float m_playerHeight = 0;
        float m_resourceHeight = 0;
        float m_moveSpeed = 0.015;
        glm::vec3 m_resourceSize = {0.35, 0.35, 0.35};
        float m_ressourcesRotation = 0;
        float m_ressourcesRotationSpeed = 0.0001;

        std::unordered_map<std::string, Team> m_teams;
        std::map<int, Egg> m_eggs;
        std::map<int, Player> m_players;
        std::vector<Broadcast> m_broadcasts;

        // Dict of all the ressources meshes and offsets on the tiles
        std::map<RessourceType, glm::vec3> m_ressourceOffset;
        std::map<RessourceType, const std::shared_ptr<StaticMesh>> m_ressourceMesh;
        std::vector<Tile> m_tilesDecor;

        // Dict of all the player meshes and animations
        std::map<std::string, std::shared_ptr<StaticMesh>> m_tilesMeshes;
        std::map<std::string, std::pair<std::shared_ptr<SkeletalMesh>, glm::vec3>> m_playerMeshes;
        std::map<std::string, std::shared_ptr<Animation>> m_playerAnims;
        std::shared_ptr<StaticMesh> m_broadcastMesh;
        std::vector<GLuint> m_resIcons;
        std::shared_ptr<StaticMesh> m_eggMesh;

        std::vector<LogMessage> m_logs;

        int m_socket = 0;
        void connectToServer(int port);

        void updateMap(const std::string& bufferView);
        void updatePlayers(const std::string& bufferView);
        void updateEggs(const std::string& bufferView);
        void addBroadcasts();
        static glm::ivec2 parseMapSize(const std::string& bufferView);
        void parseConnectionResponse();
        void updatePlayersAnim();
        void loadPlayer(const std::string& playerName, glm::vec3 scale);
        void createTiles();
        void drawUi() noexcept;
        void addEggs();

        void createScene();
        void createPlayers();
        void createRessources();
};
