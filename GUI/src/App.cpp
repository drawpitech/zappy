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
    createIslands();
}

App::~App() {
    close(m_socket);
}

void App::parseConnectionResponse() {
    std::array<char, BUFFER_SIZE> buffer{};
    buffer.fill(0);
    if (read(m_socket, buffer.data(), BUFFER_SIZE) < 0)
        throw std::runtime_error("Read failed");

    const std::string bufferView(buffer.data());
    std::cout << bufferView << std::endl;

    m_mapSize = getMapSize(bufferView);
    m_map.resize(static_cast<size_t>(m_mapSize[0]), std::vector<TileContent>(static_cast<size_t>(m_mapSize[1])));
    updateMap(bufferView);

    {   // Get the speed (sgt in the bufferView)
        size_t pos = bufferView.find("sgt ");
        if (pos == std::string::npos)
            throw std::runtime_error("sgt not found in welcome bufferView");

        m_speed =  std::stoi(bufferView.substr(pos + 4, bufferView.find('\n', pos) - pos));
    }
}

void App::createIslands() {
    static std::shared_ptr<StaticMesh> islandMesh = std::make_shared<StaticMesh>("../assets/cube.obj");

    for (float i = -m_mapSize[0] / 2; i < m_mapSize[0] / 2; i++)
        for (float j = -m_mapSize[1] / 2; j < m_mapSize[1] / 2; j++)
            m_scene->staticActors.push_back(Renderer::StaticActor({islandMesh, glm::vec3(i * 5, -0.1, j * 5), glm::vec3(1.8, 0.1, 1.8), glm::vec3(0, 0, 0)}));
}

void App::updateMap(const std::string& buffer) {
    size_t pos = buffer.find("bct");
    while (pos != std::string::npos) {
        int x = std::stoi(buffer.substr(buffer.find(' ', pos) + 1, buffer.find(' ', buffer.find(' ', pos) + 1) - buffer.find(' ', pos) - 1));
        pos = buffer.find(' ', pos) + 1;
        int y = std::stoi(buffer.substr(buffer.find(' ', pos) + 1, buffer.find(' ', buffer.find(' ', pos) + 1) - buffer.find(' ', pos) - 1));
        pos = buffer.find(' ', pos) + 1;

        for (int i = 0; i < 7; i++) {
            pos = buffer.find(' ', pos) + 1;
            m_map[static_cast<size_t>(x)][static_cast<size_t>(y)].ressources[i] = std::stoi(buffer.substr(pos, buffer.find(' ', pos) - pos));
        }

        pos = buffer.find("bct", pos);
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

glm::vec2 App::getMapSize(const std::string& buffer) {
    glm::vec2 mapSize;

    size_t pos = buffer.find("msz");
    if (pos == std::string::npos)
        throw std::runtime_error("msz not found in welcome buffer");

    pos += buffer.find(' ', pos) + 1;
    mapSize[0] = std::stof(buffer.substr(pos, buffer.find(' ', pos) - pos));
    pos = buffer.find(' ', pos) + 1;
    mapSize[1] = std::stof(buffer.substr(pos, buffer.find('\n', pos) - pos));

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

            const std::string bufferView(buffer.data());
            updateMap(bufferView);
        }

        m_renderer->render(m_scene, static_cast<float>(m_speed));
    }
}
