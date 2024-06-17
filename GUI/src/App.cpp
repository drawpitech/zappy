/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** App
*/

#include "App.hpp"

#include "Models/Animations/Animation.hpp"
#include "Models/SkeletalMesh.hpp"
#include "Models/StaticMesh.hpp"
#include "Renderer/Renderer.hpp"
#include "glm/gtx/quaternion.hpp"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"

#include <memory>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <cstdio>

#define BUFFER_SIZE 1024000 // 1MB

App::App(int port) {
    connectToServer(port);
    parseConnectionResponse();

    m_renderer = std::make_unique<Renderer>();
    m_scene = std::make_shared<Renderer::Scene>();
    createScene();
}

App::~App() {
    close(m_socket);
}

void App::updatePlayers(const std::string& bufferView) {
    size_t pos = bufferView.find("pnw");
    while (pos != std::string::npos) {
        const int playerNumber = std::stoi(bufferView.substr(bufferView.find(' ', pos) + 1, bufferView.find(' ', bufferView.find(' ', pos) + 1) - bufferView.find(' ', pos) - 1));
        pos = bufferView.find(' ', pos) + 1;

        glm::ivec2 position;
        position[0] = std::stoi(bufferView.substr(bufferView.find(' ', pos) + 1, bufferView.find(' ', bufferView.find(' ', pos) + 1) - bufferView.find(' ', pos) - 1));
        pos = bufferView.find(' ', pos) + 1;
        position[1] = std::stoi(bufferView.substr(bufferView.find(' ', pos) + 1, bufferView.find(' ', bufferView.find(' ', pos) + 1) - bufferView.find(' ', pos) - 1));
        pos = bufferView.find(' ', pos) + 1;
        position = (position - m_mapSize / 2);

        const int orientation = std::stoi(bufferView.substr(bufferView.find(' ', pos) + 1, bufferView.find(' ', bufferView.find(' ', pos) + 1) - bufferView.find(' ', pos) - 1));
        pos = bufferView.find(' ', pos) + 1;

        const int level = std::stoi(bufferView.substr(bufferView.find(' ', pos) + 1, bufferView.find(' ', bufferView.find(' ', pos) + 1) - bufferView.find(' ', pos) - 1));
        pos = bufferView.find(' ', pos) + 1;
        pos = bufferView.find(' ', pos) + 1;

        const std::string teamName = bufferView.substr(pos, bufferView.find('\n', pos) - pos);

        m_players[playerNumber] = Player {
            .position = glm::vec3(static_cast<float>(position[0]) * (m_tileSize[0] + m_tileSpacing[0]), m_playerHeight, static_cast<float>(position[1]) * (m_tileSize[2] + m_tileSpacing[1])),
            .direction = glm::vec2(orientation, 0),
            .team = teamName,
            .level = level
        };

        pos = bufferView.find("pnw", pos);
        std::cout << "New player: " << playerNumber << " at " << position[0] << ", " << position[1] << " looking " << orientation << " level " << level << " in team " << teamName << "\n";
    }

    pos = bufferView.find("ppo");
    while (pos != std::string::npos) {
        const int playerNumber = std::stoi(bufferView.substr(bufferView.find(' ', pos) + 1, bufferView.find(' ', bufferView.find(' ', pos) + 1) - bufferView.find(' ', pos) - 1));
        pos = bufferView.find(' ', pos) + 1;

        glm::ivec2 position;
        position[0] = std::stoi(bufferView.substr(bufferView.find(' ', pos) + 1, bufferView.find(' ', bufferView.find(' ', pos) + 1) - bufferView.find(' ', pos) - 1));
        pos = bufferView.find(' ', pos) + 1;
        position[1] = std::stoi(bufferView.substr(bufferView.find(' ', pos) + 1, bufferView.find(' ', bufferView.find(' ', pos) + 1) - bufferView.find(' ', pos) - 1));
        pos = bufferView.find(' ', pos) + 1;
        position = (position - m_mapSize / 2);

        const int orientation = std::stoi(bufferView.substr(bufferView.find(' ', pos) + 1, bufferView.find(' ', bufferView.find(' ', pos) + 1) - bufferView.find(' ', pos) - 1));
        pos = bufferView.find(' ', pos) + 1;

        m_players[playerNumber].position = glm::vec3(static_cast<float>(position[0]) * (m_tileSize[0] + m_tileSpacing[0]), m_playerHeight, static_cast<float>(position[1]) * (m_tileSize[2] + m_tileSpacing[1]));
        m_players[playerNumber].direction = glm::vec2(orientation, 0);

        pos = bufferView.find("ppo", pos);
    }


    pos = bufferView.find("pbc");
    while (pos != std::string::npos) {
        const int playerNumber = std::stoi(bufferView.substr(bufferView.find(' ', pos) + 1, bufferView.find(' ', bufferView.find(' ', pos) + 1) - bufferView.find(' ', pos) - 1));
        pos = bufferView.find(' ', pos) + 1;

        const std::string message = bufferView.substr(bufferView.find(' ', pos) + 1, bufferView.find('\n', pos) - pos);

        std::cout << "Player " << playerNumber << " says: " << message << "\n";

        pos = bufferView.find("pbc", pos);
    }
}

void App::parseConnectionResponse() {
    std::array<char, BUFFER_SIZE> buffer{};
    buffer.fill(0);
    if (read(m_socket, buffer.data(), BUFFER_SIZE) < 0)
        throw std::runtime_error("Read failed");

    const std::string& bufferView(buffer.data());
    std::cout << bufferView << std::endl;

    m_mapSize = parseMapSize(bufferView);
    m_map.resize(static_cast<size_t>(m_mapSize[0]), std::vector<TileContent>(static_cast<size_t>(m_mapSize[1])));
    updateMap(bufferView);
    updatePlayers(bufferView);

    {   // Get the speed (sgt in the bufferView)
        size_t pos = bufferView.find("sgt ");
        if (pos == std::string::npos)
            throw std::runtime_error("sgt not found in welcome bufferView");

        m_speed =  std::stoi(bufferView.substr(pos + 4, bufferView.find('\n', pos) - pos));
    }
}

void App::createIslands() {
    static std::shared_ptr<StaticMesh> islandMesh = std::make_shared<StaticMesh>("../assets/tile.obj");
    for (int i = -m_mapSize[0] / 2; i < m_mapSize[0] / 2; i++)
        for (int j = -m_mapSize[1] / 2; j < m_mapSize[1] / 2; j++)
            m_scene->staticActors.push_back(Renderer::StaticActor({islandMesh, glm::vec3(static_cast<float>(i) * (m_tileSize[0] + m_tileSpacing[0]), (m_tileHeight), static_cast<float>(j) * (m_tileSize[2] + m_tileSpacing[1])), m_tileSize, glm::vec3(0, 0, 0)}));
}

void App::createScene() {
    m_scene->staticActors.clear();
    m_scene->animatedActors.clear();
    createIslands();

    for (int i = -m_mapSize[0] / 2; i < m_mapSize[0] / 2; i++) {
        for (int j = -m_mapSize[1] / 2; j < m_mapSize[1] / 2; j++) {
            const TileContent& tile = m_map[i + m_mapSize[0] / 2][j + m_mapSize[1] / 2];

            std::map<RessourceType, glm::vec3> ressourceOffset = {
                {FOOD, {0.2, 0.3, m_tileSize[2] * 1/8}},
                {LINEMATE, {0.50, 0.25, m_tileSize[2] * 2/8}},
                {DERAUMERE, {0.55, 0.25, m_tileSize[2] * 3/8}},
                {SIBUR, {0.55, 0.25, m_tileSize[2] * 4/8}},
                {MENDIANE, {0.55, -0.25, m_tileSize[2] * 5/8}},
                {PHIRAS, {0.55, 0.25, m_tileSize[2] * 6/8}},
                {THYSTAME, {0.55, 0.25, m_tileSize[2] * 7/8}}
            };

            static const std::map<RessourceType, const std::shared_ptr<StaticMesh>> ressourceMesh = {
                {FOOD, std::make_shared<StaticMesh>("../assets/Ressources/Gonstre.obj")},
                {LINEMATE, std::make_shared<StaticMesh>("../assets/Ressources/pink.obj")},
                {DERAUMERE, std::make_shared<StaticMesh>("../assets/Ressources/orange.obj")},
                {SIBUR, std::make_shared<StaticMesh>("../assets/Ressources/blue.obj")},
                {MENDIANE, std::make_shared<StaticMesh>("../assets/Ressources/green.obj")},
                {PHIRAS, std::make_shared<StaticMesh>("../assets/Ressources/red.obj")},
                {THYSTAME, std::make_shared<StaticMesh>("../assets/Ressources/red.obj")}
            };

            for (const auto& [ressourceType, offset] : ressourceOffset) {
                const glm::vec3 ressourcePosition = glm::vec3((static_cast<float>(i) * (m_tileSize[0] + m_tileSpacing[0])), m_resourceHeight, (static_cast<float>(j) * (m_tileSize[2] + m_tileSpacing[1])));
                static constexpr glm::vec3 ressourceRotation = glm::vec3(0, 0, 0);

                for (int nb = 0; nb < tile.ressources[ressourceType]; nb++)
                    m_scene->staticActors.push_back(
                        Renderer::StaticActor({
                            ressourceMesh.at(ressourceType),
                            ressourcePosition + glm::vec3(static_cast<float>(nb % m_ressourceLayer) * offset[0] - m_tileSize[2] / 2 + m_tileSize[2] * 1/8, glm::floor(static_cast<float>(nb / m_ressourceLayer)) * offset[1], offset[2] - m_tileSize[2] / 2),
                            m_resourceSize,
                            ressourceRotation
                        })
                    );
            }
        }
    }

    for (const auto& [playerNumber, player] : m_players) {
        static const std::shared_ptr<SkeletalMesh> playerMesh = std::make_shared<SkeletalMesh>("../assets/Dan/Dancing Twerk.dae");
        static const std::shared_ptr<Animation> playerAnim = std::make_shared<Animation>("../assets/Dan/Dancing Twerk.dae", playerMesh);
        static constexpr glm::vec3 playerScale = glm::vec3(100, 100, 100);
        static constexpr glm::vec3 playerRotation = glm::vec3(0, 0, 0);

        m_scene->animatedActors.push_back({playerMesh, std::make_shared<Animator>(playerAnim), player.position, playerScale, playerRotation});

    }
}

void App::updateMap(const std::string& bufferView) {
    size_t pos = bufferView.find("bct");
    while (pos != std::string::npos) {
        int x = std::stoi(bufferView.substr(bufferView.find(' ', pos) + 1, bufferView.find(' ', bufferView.find(' ', pos) + 1) - bufferView.find(' ', pos) - 1));
        pos = bufferView.find(' ', pos) + 1;
        int y = std::stoi(bufferView.substr(bufferView.find(' ', pos) + 1, bufferView.find(' ', bufferView.find(' ', pos) + 1) - bufferView.find(' ', pos) - 1));
        pos = bufferView.find(' ', pos) + 1;

        for (int i = 0; i < 7; i++) {
            pos = bufferView.find(' ', pos) + 1;
            m_map[static_cast<size_t>(x)][static_cast<size_t>(y)].ressources[i] = std::stoi(bufferView.substr(pos, bufferView.find(' ', pos) - pos));
        }

        pos = bufferView.find("bct", pos);
    }
}

void App::connectToServer(int port) {
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket < 0)
        throw std::runtime_error("Socket creation failed");

    sockaddr_in server_addr {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr = { .s_addr = INADDR_ANY },
        .sin_zero = { 0 }
    };

    if (connect(m_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) < 0)
        throw std::runtime_error("Connection failed");

    std::array<char, BUFFER_SIZE> buffer{};
    if (read(m_socket, buffer.data(), BUFFER_SIZE) < 0)
        throw std::runtime_error("Read failed");

    if (std::string(buffer.data()) != "WELCOME\n")
        throw std::runtime_error("Connection failed");

    if (write(m_socket, "GRAPHIC\n", 8) < 0)
        throw std::runtime_error("Write failed");
}

glm::ivec2 App::parseMapSize(const std::string& bufferView) {
    glm::ivec2 mapSize;

    size_t pos = bufferView.find("msz");
    if (pos == std::string::npos)
        throw std::runtime_error("msz not found in welcome buffer");

    pos += bufferView.find(' ', pos) + 1;
    mapSize[0] = std::stoi(bufferView.substr(pos, bufferView.find(' ', pos) - pos));
    pos = bufferView.find(' ', pos) + 1;
    mapSize[1] = std::stoi(bufferView.substr(pos, bufferView.find('\n', pos) - pos));

    return mapSize;
}

void App::run() {

    // Animated mesh example

    while (!m_renderer->shouldStop()) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(m_socket, &readfds);
        timeval timeout { .tv_sec = 0, .tv_usec = 0 };
        if (select(m_socket + 1, &readfds, nullptr, nullptr, &timeout) > 0) {
            std::array<char, BUFFER_SIZE> buffer{};
            buffer.fill(0);
            if (read(m_socket, buffer.data(), BUFFER_SIZE) < 0)
                throw std::runtime_error("Read failed");

            const std::string& bufferView(buffer.data());

            updateMap(bufferView);
            updatePlayers(bufferView);
            createScene();
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        m_renderer->render(m_scene, static_cast<float>(m_speed));
    }
}
