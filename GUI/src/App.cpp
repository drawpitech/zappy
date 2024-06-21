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
#include "imgui.h"

#include <chrono>
#include <iostream>
#include <memory>
#include <netinet/in.h>

App::App(int port) {
    m_renderer = std::make_unique<Renderer>();
    m_scene = std::make_shared<Renderer::Scene>();

    {   // Load the meshes and animations
        m_playerMeshes["Dan"] = std::make_shared<SkeletalMesh>("assets/Dan/Dan.dae");
        m_playerAnims["DanIdle"] = std::make_shared<Animation>("assets/Dan/Idle/Idle.dae", m_playerMeshes["Dan"]);
        m_playerAnims["DanEat"] = std::make_shared<Animation>("assets/Dan/Eat/Drinking.dae", m_playerMeshes["Dan"]);
        m_playerAnims["DanMove"] = std::make_shared<Animation>("assets/Dan/Move/Fast Run.dae", m_playerMeshes["Dan"]);
        m_playerAnims["DanRitual"] = std::make_shared<Animation>("assets/Dan/Ritual/Tut Hip Hop Dance.dae", m_playerMeshes["Dan"]);
        m_playerAnims["DanBirth"] = std::make_shared<Animation>("assets/Dan/Birth/Standing Taunt Battlecry.dae", m_playerMeshes["Dan"]);

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

        m_islandMesh = std::make_shared<StaticMesh>("assets/tile.obj");
        m_broadcastMesh = std::make_shared<StaticMesh>("assets/broadcast.obj");
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
}

App::~App() {
    close(m_socket);
}

void App::createScene() {
    m_scene->staticActors.clear();
    m_scene->animatedActors.clear();

    // Create all island tiles and ressources
    for (int i = -m_mapSize[0] / 2; i < m_mapSize[0] / 2; i++) {
        for (int j = -m_mapSize[1] / 2; j < m_mapSize[1] / 2; j++) {
            const TileContent& tile = m_map[i + m_mapSize[0] / 2][j + m_mapSize[1] / 2];

            // Add the island tile
            m_scene->staticActors.push_back(Renderer::StaticActor({m_islandMesh, glm::vec3(static_cast<float>(i) * (m_tileSize[0] + m_tileSpacing[0]), m_tileHeight, static_cast<float>(j) * (m_tileSize[1] + m_tileSpacing[1])), m_tileSize, glm::vec3(0, 0, 0)}));

            // Display the ressources
            for (const auto& [ressourceType, offset] : m_ressourceOffset) {
                const glm::vec3 ressourcePosition = glm::vec3((static_cast<float>(i) * (m_tileSize[0] + m_tileSpacing[0])), m_resourceHeight, (static_cast<float>(j) * (m_tileSize[1] + m_tileSpacing[1])));
                static constexpr glm::vec3 ressourceRotation = glm::vec3(0, 0, 0);

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
    if (ImGui::Begin("Mesh and Animation Selection")) {
        for (auto& [teamName, team] : m_teams) {
            ImGui::Text("%s", teamName.c_str());
            const char* currentMesh = team.mesh.first.c_str();
            if (ImGui::BeginCombo("Mesh", currentMesh)) {
                for (auto& [playerName, playerMesh] : m_playerMeshes) {
                    bool isSelected = (team.mesh.second == playerMesh);
                    if (ImGui::Selectable(playerName.c_str(), isSelected)) {
                        team.mesh.second = playerMesh;
                        team.mesh.first = playerName;
                        createScene();
                    }
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
        }

        ImGui::End();
    }

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
        printf("Grod pd\n");
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
        static constexpr glm::vec3 playerScale = glm::vec3(100, 100, 100);
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
        m_scene->animatedActors.push_back({m_teams[player.teamName].mesh.second, player.animator, player.position + player.visualPositionOffset, playerScale, playerRotation});
    }
}

void App::updatePlayersAnim() { // NOLINT
    for (auto& [playerNumber, player] : m_players) {
        std::string meshName = m_teams[player.teamName].mesh.first;
        if (player.currentAnim == IDLE) {
            player.animator = std::make_shared<Animator>(m_playerAnims[meshName + "Idle"]);
            player.currentAnim = DEFAULT;
            createPlayers();
        }
        if (player.currentAnim == RITUAL) {
            player.animator = std::make_shared<Animator>(m_playerAnims[meshName + "Ritual"]);
            player.currentAnim = DEFAULT;
            createPlayers();
        }
        if (player.currentAnim == BIRTH) {
            player.animator = std::make_shared<Animator>(m_playerAnims[meshName + "Birth"]);
            player.currentAnim = DEFAULT;
            player.currentAction = BIRTH;
            createPlayers();
        }
        if (player.currentAnim == MOVE) {
            player.animator = std::make_shared<Animator>(m_playerAnims[meshName + "Move"]);
            player.currentAnim = DEFAULT;
            player.currentAction = MOVE;
            player.visualPositionOffset = -player.moveOrientation;
            createPlayers();
        }
        if (player.currentAction == BIRTH) {
            if (player.animator->isAnimationDone()) {
                player.currentAction = IDLE;
                player.currentAnim = IDLE;
                createPlayers();
            }
        }
        if (player.currentAction == MOVE) {
            if (player.moveOrientation[0] > 0) {
                if (player.visualPositionOffset[0] < 0)
                    player.visualPositionOffset[0] += (m_tileSpacing[0] + m_tileSize[0]) * 0.001 * m_speed; // NOLINT
                else {
                    player.visualPositionOffset = {0, 0, 0};
                    player.currentAction = IDLE;
                    player.currentAnim = IDLE;
                }
            }
            if (player.moveOrientation[0] < 0) {
                if (player.visualPositionOffset[0] > 0)
                    player.visualPositionOffset[0] -= (m_tileSpacing[0] + m_tileSize[0]) * 0.001 * m_speed; // NOLINT
                else {
                    player.visualPositionOffset = {0, 0, 0};
                    player.currentAction = IDLE;
                    player.currentAnim = IDLE;
                }
            }

            if (player.moveOrientation[2] > 0) {
                if (player.visualPositionOffset[2] < 0)
                    player.visualPositionOffset[2] += (m_tileSpacing[1] + m_tileSize[2]) * 0.001 * m_speed; // NOLINT
                else {
                    player.visualPositionOffset = {0, 0, 0};
                    player.currentAction = IDLE;
                    player.currentAnim = IDLE;
                }
            }
            if (player.moveOrientation[2] < 0) {
                if (player.visualPositionOffset[2] > 0)
                    player.visualPositionOffset[2] -= (m_tileSpacing[1] + m_tileSize[2]) * 0.001 * m_speed; // NOLINT
                else {
                    player.visualPositionOffset = {0, 0, 0};
                    player.currentAction = IDLE;
                    player.currentAnim = IDLE;
                }
            }
            createPlayers();
        }
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
                glm::vec3(m_broadcasts[i].position),
                glm::vec3(elapsedTime / 50, 1, elapsedTime / 50),
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
            updatePlayers(bufferView);
            updateEggs(bufferView);
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
    }

    // TODO: remove this
    // ImGui::SaveIniSettingsToDisk("../imgui.ini");
}
