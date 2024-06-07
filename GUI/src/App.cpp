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
}

App::~App() {
    close(m_socket);
}

void App::parseConnectionResponse() {
    std::array<char, BUFFER_SIZE> buffer{};
    buffer.fill(0);
    if (read(m_socket, buffer.data(), BUFFER_SIZE) < 0) {
        close(m_socket);
        throw std::runtime_error("Read failed");
    }

    std::cout << buffer.data() << std::endl;

    m_mapSize = getMapSize(buffer.data());
    m_map.resize(static_cast<size_t>(m_mapSize[0]), std::vector<TileContent>(static_cast<size_t>(m_mapSize[1])));
    parseMap(buffer.data());
}

void App::parseMap(const std::string& buffer) {
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

    if (connect(m_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) < 0) {
        close(m_socket);
        throw std::runtime_error("Connection failed");
    }

    std::array<char, BUFFER_SIZE> buffer{};
    if (read(m_socket, buffer.data(), BUFFER_SIZE) < 0) {
        close(m_socket);
        throw std::runtime_error("Read failed");
    }

    if (std::string(buffer.data()) != "WELCOME\n")
        throw std::runtime_error("Connection failed");

    if (write(m_socket, "GRAPHIC\n", 8) < 0) {
        close(m_socket);
        throw std::runtime_error("Write failed");
    }
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
    std::shared_ptr<Renderer::Scene> scene = std::make_shared<Renderer::Scene>();
    scene->staticMeshes.emplace_back(std::make_shared<StaticMesh>("../assets/SponzaPBR/Sponza.gltf"));

    // Animated mesh example
    const std::shared_ptr<SkeletalMesh> danMesh = std::make_shared<SkeletalMesh>("../assets/Dancing_Twerk/Dancing Twerk.dae");
    const std::shared_ptr<Animation> idleAnim = std::make_shared<Animation>("../assets/Dancing_Twerk/Dancing Twerk.dae", danMesh);
    scene->animatedMeshes.push_back(std::make_shared<Renderer::AnimatedMesh>(danMesh, idleAnim));
    scene->animatedMeshes[0]->mesh->setScale(glm::vec3(100, 100, 100));

    while (!m_renderer->shouldStop()) {
        // Model rotation exemple
        glm::vec3 rotation = scene->staticMeshes[0]->getRotation();
        rotation[1] += 0.1;
        scene->staticMeshes[0]->setRotation(rotation);

        m_renderer->render(scene);
    }
}
