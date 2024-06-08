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
        position = (position - m_mapSize / 2) * 5;

        const int orientation = std::stoi(bufferView.substr(bufferView.find(' ', pos) + 1, bufferView.find(' ', bufferView.find(' ', pos) + 1) - bufferView.find(' ', pos) - 1));
        pos = bufferView.find(' ', pos) + 1;

        const int level = std::stoi(bufferView.substr(bufferView.find(' ', pos) + 1, bufferView.find(' ', bufferView.find(' ', pos) + 1) - bufferView.find(' ', pos) - 1));
        pos = bufferView.find(' ', pos) + 1;
        pos = bufferView.find(' ', pos) + 1;

        const std::string teamName = bufferView.substr(pos, bufferView.find('\n', pos) - pos);

        m_players[playerNumber] = Player {
            .position = glm::vec3(position[0], 0.5, position[1]),
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
        position = (position - m_mapSize / 2) * 5;

        const int orientation = std::stoi(bufferView.substr(bufferView.find(' ', pos) + 1, bufferView.find(' ', bufferView.find(' ', pos) + 1) - bufferView.find(' ', pos) - 1));
        pos = bufferView.find(' ', pos) + 1;

        m_players[playerNumber].position = glm::vec3(position[0], 0.5, position[1]);
        m_players[playerNumber].direction = glm::vec2(orientation, 0);

        pos = bufferView.find("ppo", pos);
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
    static std::shared_ptr<StaticMesh> islandMesh = std::make_shared<StaticMesh>("../assets/cube.obj");

    for (int i = -m_mapSize[0] / 2; i < m_mapSize[0] / 2; i++)
        for (int j = -m_mapSize[1] / 2; j < m_mapSize[1] / 2; j++)
            m_scene->staticActors.push_back(Renderer::StaticActor({islandMesh, glm::vec3(i * 5, -0.1, j * 5), glm::vec3(1.8, 0.1, 1.8), glm::vec3(0, 0, 0)}));
}

void App::createScene() {
    m_scene->staticActors.clear();
    createIslands();

    for (int i = -m_mapSize[0] / 2; i < m_mapSize[0] / 2; i++) {
        for (int j = -m_mapSize[1] / 2; j < m_mapSize[1] / 2; j++) {
            const TileContent& tile = m_map[i + m_mapSize[0] / 2][j + m_mapSize[1] / 2];

            static const std::map<RessourceType, glm::vec2> ressourceOffset = {
                {FOOD, {-1.5, -1.5}},
                {LINEMATE, {-1, -1.5}},
                {DERAUMERE, {0.5, -1.5}},
                {SIBUR, {0, -1.5}},
                {MENDIANE, {0.5, -1.5}},
                {PHIRAS, {1, -1.5}},
                {THYSTAME, {1.5, -1.5}}
            };

            static const std::map<RessourceType, const std::shared_ptr<StaticMesh>> ressourceMesh = {
                {FOOD, std::make_shared<StaticMesh>("../assets/cube.obj")},
                {LINEMATE, std::make_shared<StaticMesh>("../assets/cube.obj")},
                {DERAUMERE, std::make_shared<StaticMesh>("../assets/cube.obj")},
                {SIBUR, std::make_shared<StaticMesh>("../assets/cube.obj")},
                {MENDIANE, std::make_shared<StaticMesh>("../assets/cube.obj")},
                {PHIRAS, std::make_shared<StaticMesh>("../assets/cube.obj")},
                {THYSTAME, std::make_shared<StaticMesh>("../assets/cube.obj")}
            };

            for (const auto& [ressourceType, offset] : ressourceOffset) {
                const glm::vec3 ressourcePosition = glm::vec3((static_cast<float>(i) * 5) + offset[0], 0.5, (static_cast<float>(j) * 5 + offset[1]));
                static constexpr glm::vec3 ressourceScale = glm::vec3(0.1, 0.1, 0.1);
                static constexpr glm::vec3 ressourceRotation = glm::vec3(0, 0, 0);

                for (int f = 0; f < tile.ressources[ressourceType]; f++)
                    m_scene->staticActors.push_back(
                        Renderer::StaticActor({
                            ressourceMesh.at(ressourceType),
                            ressourcePosition + glm::vec3(0, 0.5 * f, 0),
                            ressourceScale, ressourceRotation
                        })
                    );
            }
        }
    }

    for (const auto& [playerNumber, player] : m_players) {
        static const std::shared_ptr<StaticMesh> playerMesh = std::make_shared<StaticMesh>("../assets/cube.obj");
        static constexpr glm::vec3 playerScale = glm::vec3(0.5, 1, 0.5);
        static constexpr glm::vec3 playerRotation = glm::vec3(0, 0, 0);

        m_scene->staticActors.push_back(Renderer::StaticActor({playerMesh, player.position, playerScale, playerRotation}));
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
    // m_scene->staticMeshes.emplace_back(std::make_shared<StaticMesh>("../assets/SponzaPBR/Sponza.gltf"));

    // Animated mesh example
    const std::shared_ptr<SkeletalMesh> danMesh = std::make_shared<SkeletalMesh>("../assets/Dancing_Twerk/Dancing Twerk.dae");
    const std::shared_ptr<Animation> danceAnim = std::make_shared<Animation>("../assets/Dancing_Twerk/Dancing Twerk.dae", danMesh);
    m_scene->animatedActors.push_back({danMesh, std::make_shared<Animator>(danceAnim), glm::vec3(0, 0, 0.5), glm::vec3(100, 100, 100), glm::vec3(0, 0, 0)});

    m_scene->animatedActors.push_back({danMesh, std::make_shared<Animator>(danceAnim), glm::vec3(0.5, 0, 0), glm::vec3(100, 100, 100), glm::vec3(0, 0, 0)});
    m_scene->animatedActors.push_back({danMesh, std::make_shared<Animator>(danceAnim), glm::vec3(-0.5, 0, 0), glm::vec3(100, 100, 100), glm::vec3(0, 0, 0)});

    m_scene->animatedActors.push_back({danMesh, std::make_shared<Animator>(danceAnim), glm::vec3(0, 0, -0.5), glm::vec3(100, 100, 100), glm::vec3(0, 0, 0)});
    m_scene->animatedActors.push_back({danMesh, std::make_shared<Animator>(danceAnim), glm::vec3(1, 0, -0.5), glm::vec3(100, 100, 100), glm::vec3(0, 0, 0)});
    m_scene->animatedActors.push_back({danMesh, std::make_shared<Animator>(danceAnim), glm::vec3(-1, 0, -0.5), glm::vec3(100, 100, 100), glm::vec3(0, 0, 0)});

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

        m_renderer->render(m_scene, static_cast<float>(m_speed));
    }
}
