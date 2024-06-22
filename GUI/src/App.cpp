/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** App
*/

#include "App.hpp"

#include "Utils.hpp"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "glm/common.hpp"
#include "glm/ext/vector_float3.hpp"
#include "imgui.h"

#include <chrono>
#include <cstdio>
#include <memory>
#include <netinet/in.h>

App::App(int port) {
    m_renderer = std::make_unique<Renderer>();
    m_scene = std::make_shared<Renderer::Scene>();

    {   // Load the meshes and animations
        loadPlayer("Dan", glm::vec3(150, 150, 150));
        loadPlayer("Quentin", glm::vec3(100, 100, 100));
        m_ressourceOffset = {
            {FOOD, {0.2, 0.3, m_tileSize[2] * 1/8}},
            {LINEMATE, {0.50, 0.25, m_tileSize[2] * 2/8}},
            {DERAUMERE, {0.55, 0.25, m_tileSize[2] * 3/8}},
            {SIBUR, {0.55, 0.25, m_tileSize[2] * 4/8}},
            {MENDIANE, {0.55, -0.25, m_tileSize[2] * 5/8}},
            {PHIRAS, {0.55, 0.25, m_tileSize[2] * 6/8}},
            {THYSTAME, {0.55, 0.25, m_tileSize[2] * 7/8}}
        };

        m_ressourceMesh = {
            {FOOD, std::make_shared<StaticMesh>("assets/Ressources/Gonstre.obj")},
            {LINEMATE, std::make_shared<StaticMesh>("assets/Ressources/pink.obj")},
            {DERAUMERE, std::make_shared<StaticMesh>("assets/Ressources/orange.obj")},
            {SIBUR, std::make_shared<StaticMesh>("assets/Ressources/blue.obj")},
            {MENDIANE, std::make_shared<StaticMesh>("assets/Ressources/green.obj")},
            {PHIRAS, std::make_shared<StaticMesh>("assets/Ressources/red.obj")},
            {THYSTAME, std::make_shared<StaticMesh>("assets/Ressources/purple.obj")}
        };

        m_tilesMeshes["white"] = std::make_shared<StaticMesh>("assets/whiteRock.gltf");
        m_tilesMeshes["black"] = std::make_shared<StaticMesh>("assets/greyRock.gltf");
        m_broadcastMesh = std::make_shared<StaticMesh>("assets/broadcast.obj");
        m_eggMesh = std::make_shared<StaticMesh>("assets/egg/scene.gltf");
    }

    static const std::vector<std::string> resIconsFilepaths = {
        "assets/Ressources Icons/gonstre.png",
        "assets/Ressources Icons/pink.png",
        "assets/Ressources Icons/orange.png",
        "assets/Ressources Icons/blue.png",
        "assets/Ressources Icons/green.png",
        "assets/Ressources Icons/pruple.png",
        "assets/Ressources Icons/red.png",
    };

    m_resIcons = Utils::Instance<Utils::ImageLoader, const std::vector<std::string>&>::Get(resIconsFilepaths)->getImages();
    connectToServer(port);
    parseConnectionResponse();
    createTiles();
}

void App::loadPlayer(const std::string& playerName, glm::vec3 scale) {
    m_playerMeshes[playerName].first = std::make_shared<SkeletalMesh>("assets/Players/" + playerName + "/" + playerName + ".dae");
    m_playerMeshes[playerName].second = scale;
    m_playerAnims[playerName + "Idle"] = std::make_shared<Animation>("assets/Players/" + playerName + "/Idle.dae", m_playerMeshes[playerName].first);
    m_playerAnims[playerName + "Move"] = std::make_shared<Animation>("assets/Players/" + playerName + "/Move.dae", m_playerMeshes[playerName].first);
    m_playerAnims[playerName + "Ritual"] = std::make_shared<Animation>("assets/Players/" + playerName + "/Ritual.dae", m_playerMeshes[playerName].first);
    m_playerAnims[playerName + "Birth"] = std::make_shared<Animation>("assets/Players/" + playerName + "/Birth.dae", m_playerMeshes[playerName].first);
}

void App::createTiles() {
    for (int i = -m_mapSize[0] / 2; i < m_mapSize[0] / 2; i++) {
        for (int j = -m_mapSize[1] / 2; j < m_mapSize[1] / 2; j++) {
            int randomHight = rand() % 10 + m_mapSize[0] / 2 + m_mapSize[1] / 2;
            int centerHight = abs(i) + abs(j);
            for (int k = 0; k < randomHight - centerHight; k++)
                m_tilesDecor.push_back(
                    Tile {
                        .position = glm::vec3((static_cast<float>(i) * (m_tileSize[0] * 2 + m_tileSpacing[0])), m_tileHeight - m_tileSize[0] * 2 * static_cast<float>(k), (static_cast<float>(j) * (m_tileSize[1] * 2 + m_tileSpacing[1]))),
                        .mesh = m_tilesMeshes[(i + j) % 2 == 0 ? "white" : "black"]
                    }
                );
        }
    }
}

App::~App() {
    close(m_socket);
}

void App::createScene() {
    m_scene->staticActors.clear();
    m_scene->animatedActors.clear();

    // Island tiles creation
    for (const auto& tile : m_tilesDecor) {
        m_scene->staticActors.push_back(Renderer::StaticActor({
            .mesh = tile.mesh,
            .position = tile.position,
            .scale = m_tileSize,
            .rotation = glm::vec3(0, 0, 0)
        }));
    }

    // Eggs creation
    for (const auto& egg : m_eggs) {
        m_scene->staticActors.push_back(Renderer::StaticActor({
            .mesh = m_eggMesh,
            .position = egg.second.position,
            .scale = glm::vec3(0.1),
            .rotation = glm::vec3(270, 0, 0)
        }));
    }

    // Create all island tiles and ressources
    for (int i = -m_mapSize[0] / 2; i < m_mapSize[0] / 2; i++) {
        for (int j = -m_mapSize[1] / 2; j < m_mapSize[1] / 2; j++) {
            const TileContent& tile = m_map[i + m_mapSize[0] / 2][j + m_mapSize[1] / 2];

            // Display the ressources
            for (const auto& [ressourceType, offset] : m_ressourceOffset) {
                const glm::vec3 ressourcePosition = glm::vec3((static_cast<float>(i) * (m_tileSize[0] + m_tileSpacing[0])), m_resourceHeight, (static_cast<float>(j) * (m_tileSize[1] + m_tileSpacing[1])));
                glm::vec3 ressourceRotation = glm::vec3(0, m_ressourcesRotation, 0);
                m_ressourcesRotation += m_ressourcesRotationSpeed;
                if (m_ressourcesRotation > 360)
                    m_ressourcesRotation = 0;

                for (int nb = 0; nb < tile.ressources[ressourceType]; nb++)
                    m_scene->staticActors.push_back(
                        Renderer::StaticActor({
                            m_ressourceMesh.at(ressourceType),
                            ressourcePosition + glm::vec3(static_cast<float>(nb % m_ressourceLayer) * offset[0] - m_tileSize[2] / 2 + m_tileSize[2] * 1/8, glm::floor(static_cast<float>(nb) / static_cast<float>(m_ressourceLayer)) * offset[1], offset[2] - m_tileSize[2] / 2),
                            m_resourceSize,
                            ressourceRotation
                        })
                    );
            }
        }
    }

    createPlayers();
    addBroadcasts();
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

    LOG("Connected to server", GREEN);
}

void App::drawUi() noexcept {   // NOLINT
    // Mesh selection
    ImGui::Begin("Mesh and Animation Selection");
    for (auto& [teamName, team] : m_teams) {
        ImGui::Text("%s", teamName.c_str());
        const char* currentMesh = team.mesh.first.c_str();
        if (ImGui::BeginCombo(("Mesh##" + teamName).c_str(), currentMesh)) {
            for (auto& [playerName, playerMesh] : m_playerMeshes) {
                bool isSelected = (team.mesh.second == playerMesh.first);
                if (ImGui::Selectable(playerName.c_str(), isSelected)) {
                    team.mesh.second = playerMesh.first;
                    team.mesh.first = playerName;
                    updatePlayersAnim();
                    createScene();
                }
                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }

    ImGui::End();

    ImGui::Begin("Trantorians");
    for (auto& [id, player] : m_players)
    {
        ImGui::Text("%s", (std::to_string(id) + " Trantorian " + player.teamName).c_str());
        ImGui::Text("LEVEL: %d\n", player.level);
        if (ImGui::TreeNode((std::to_string(id) + " Inventory").c_str()))
        {
            for (std::size_t i = 0; i < RESNUMBER; ++i)
            {
                ImGui::Image(reinterpret_cast<ImTextureID>(m_resIcons[i]), {60, 60}, ImVec2(0, 1), ImVec2(1, 0));   // NOLINT
                ImGui::SameLine();
                ImGui::Text("%d", player.inv.ressources[i]);
            }

            ImGui::TreePop();
        }
        ImGui::Separator();
    }
    ImGui::End();

    static int freq{};
    ImGui::Begin("Parameters");
    ImGui::InputInt("Frequency", &freq);
    if (ImGui::Button("Send request"))
    {
        dprintf(m_socket, "sst %d\n", freq);
        m_speed = freq;
    }
    ImGui::End();

    // Add logs to the UI
    ImGui::Begin("Logs");
    for (const auto& log : m_logs) {
        // Make the time a string
        std::array<char, 9> timeStr{};
        strftime(timeStr.data(), 9, "%H:%M:%S", localtime(&log.getTime()));

        // Display the log
        ImGui::TextColored(log.getColor(), "[%s] %s", timeStr.data(), log.getMessage().c_str());
        ImGui::SetScrollHereY(1);   // To scroll to the bottom
    }
    ImGui::End();
}

void App::createPlayers() {
    m_scene->animatedActors.clear();
    for (auto& [playerNumber, player] : m_players) {
        int newOrientation = 1;
        if (player.orientation == 1)
            newOrientation = 3;
        else if (player.orientation == 2)
            newOrientation = 2;
        else if (player.orientation == 3)
            newOrientation = 1;
        else if (player.orientation == 4)
            newOrientation = 0;
        const glm::vec3 playerRotation = glm::vec3(0, (newOrientation - 1) * 90, 0);
        //std::cout << m_playerMeshes[player.teamName].second.x << " " << m_playerMeshes[player.teamName].second.y << " " << m_playerMeshes[player.teamName].second.z << std::endl;
        //std::cout << m_teams[player.teamName].mesh.first << std::endl;
        //std::cout << m_playerMeshes[player.teamName].second.x << " " << m_playerMeshes[player.teamName].second.y << " " << m_playerMeshes[player.teamName].second.z << std::endl;
        m_scene->animatedActors.push_back({
            m_teams[player.teamName].mesh.second,
            player.animator,
            player.position + player.visualPositionOffset,
            m_playerMeshes[m_teams[player.teamName].mesh.first].second,
            playerRotation,
            glm::vec3(1, 1, 1)
        });
    }
}

void App::addBroadcasts() {
    for (int i = 0; i < m_broadcasts.size(); i++) {
        std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
        float elapsedTime = std::chrono::duration<float, std::milli>(currentTime - m_broadcasts[i].startTime).count();

        if (elapsedTime > 2000)
            m_broadcasts.erase(m_broadcasts.begin() + i);
        else {
            m_scene->staticActors.push_back({
                m_broadcastMesh,
                glm::vec3(m_broadcasts[i].position) + glm::vec3(0, 0.5, 0),
                glm::vec3(elapsedTime / 50, 1.5, elapsedTime / 50),
                glm::vec3(0, 0, 0)
            });
        }
    }
}

void App::run() {
    // Load imgui settings
    ImGui::LoadIniSettingsFromDisk("../imgui.ini");

    // Buffer to store the data received from the server
    std::array<char, BUFFER_SIZE> buffer{};

    while (!m_renderer->shouldStop()) {
        m_startFrameTime = std::chrono::high_resolution_clock::now();
        // Check if there is data to read from the server
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(m_socket, &readfds);
        timeval timeout { .tv_sec = 0, .tv_usec = 0 };
        if (select(m_socket + 1, &readfds, nullptr, nullptr, &timeout) > 0) {
            // Read the data from the server
            buffer.fill(0);
            size_t readSize = 0;
            while (readSize < BUFFER_SIZE) {
                readSize += read(m_socket, buffer.data() + readSize, BUFFER_SIZE - readSize);
                if (buffer[readSize - 1] == '\n')
                    break;
            }

            const std::string& bufferView(buffer.data());

            // Update the map and players
            updateMap(bufferView);
            updateEggs(bufferView);
            updatePlayers(bufferView);
        }

        // Begin UI rendering
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport();

        // Render the scene
        drawUi();
        updatePlayersAnim();
        createScene();
        m_renderer->render(m_scene, static_cast<float>(m_speed));
        m_endFrameTime = std::chrono::high_resolution_clock::now();
        m_frameTime = std::chrono::duration<float, std::milli>(m_endFrameTime - m_startFrameTime).count();
    }

    // TODO: remove this
    // ImGui::SaveIniSettingsToDisk("../imgui.ini");
}
