/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** App
*/

#include "App.hpp"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"

#include <netinet/in.h>

App::App(int port) {
    m_renderer = std::make_unique<Renderer>();
    m_scene = std::make_shared<Renderer::Scene>();

    {   // Load the meshes and animations
        m_playerMeshes["Dan"] = std::make_shared<SkeletalMesh>("assets/Dan/Dancing Twerk.dae");
        m_playerAnims["Twerk"] = std::make_shared<Animation>("assets/Dan/Dancing Twerk.dae", m_playerMeshes["Dan"]);
        m_playerMeshes["Quentin"] = std::make_shared<SkeletalMesh>("assets/Quentin/Breakdance Uprock Var 2.dae");
        m_playerAnims["Breakdance"] = std::make_shared<Animation>("assets/Quentin/Breakdance Uprock Var 2.dae", m_playerMeshes["Quentin"]);

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
            {THYSTAME, std::make_shared<StaticMesh>("assets/Ressources/red.obj")}
        };

        m_islandMesh = std::make_shared<StaticMesh>("assets/tile.obj");
    }

    connectToServer(port);
    parseConnectionResponse();
    createScene();
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


    // Display the players
    for (const auto& [playerNumber, player] : m_players) {
        static constexpr glm::vec3 playerScale = glm::vec3(100, 100, 100);
        const glm::vec3 playerRotation = glm::vec3(0, (player.orientation - 1) * 90, 0);

        m_scene->animatedActors.push_back({m_teams[player.teamName].mesh.second, player.animator, player.position, playerScale, playerRotation});
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

    LOG("Connected to server", GREEN);
}

void App::drawUi() noexcept {
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
            createScene();
        }

        // Begin UI rendering
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport();

        // Render the scene
        drawUi();
        m_renderer->render(m_scene, static_cast<float>(m_speed));
    }

    // TODO: remove this
    // ImGui::SaveIniSettingsToDisk("../imgui.ini");
}
