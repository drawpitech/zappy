/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** Parsing
*/

#include "App.hpp"

#include <ctime>
#include <stdexcept>
#include <thread>
#include <chrono>
#include <algorithm>
#include <iostream>
#include <ctime>

#define PARSER_INIT() \
    size_t pos = 0;

#define PARSER_FIRST_SYMBOL(symbol) \
    pos = bufferView.find(symbol);

#define PARSER_NEXT_SYMBOL(symbol) \
    pos = bufferView.find(symbol, pos);

#define PARSER_NEXT_INT(buffer, pos) \
    std::stoi((buffer).substr((buffer).find(' ', pos) + 1, (buffer).find(' ', (buffer).find(' ', pos) + 1) - (buffer).find(' ', pos) - 1)); \
    (pos) = (buffer).find(' ', pos) + 1;

#define PARSER_LAST_INT(buffer, pos) \
    std::stoi((buffer).substr((buffer).find(' ', pos) + 1, (buffer).find('\n', pos) - (pos)));  \
    (pos) = (buffer).find('\n', pos) + 1;

#define PARSER_NEXT_STRING(buffer, pos) \
    buffer.substr((buffer).find(' ', pos) + 1, (buffer).find(' ', (buffer).find(' ', pos) + 1) - (buffer).find(' ', pos) - 1);  \
    (pos) = (buffer).find(' ', pos) + 1;

#define PARSER_LAST_STRING(buffer, pos) \
    buffer.substr((buffer).find(' ', pos) + 1, (buffer).find('\n', pos) - (buffer).find(' ', pos) - 1); \
    (pos) = (buffer).find('\n', pos) + 1;

void App::updatePlayers(const std::string& bufferView) {    // NOLINT
    // Player new connection (pnw playerNumber x y orientation level teamName)
    PARSER_INIT();
    PARSER_FIRST_SYMBOL("pnw");
    while (pos != std::string::npos) {
        glm::ivec2 position;

        const int playerNumber = PARSER_NEXT_INT(bufferView, pos);
        position[0] = PARSER_NEXT_INT(bufferView, pos);
        position[1] = PARSER_NEXT_INT(bufferView, pos);
        const int orientation = PARSER_NEXT_INT(bufferView, pos);
        const int level = PARSER_NEXT_INT(bufferView, pos);
        const std::string teamName = PARSER_LAST_STRING(bufferView, pos);

        position = (position - m_mapSize / 2);

        // Player creation
        m_players[playerNumber] = Player {
            .position = glm::vec3(static_cast<float>(position[0]) * (m_tileSpacing[0] + m_tileSize[0]), m_playerHeight, static_cast<float>(position[1]) * (m_tileSpacing[1] + m_tileSize[1])),
            .orientation = orientation,
            .teamName = teamName,
            .level = level,
            .animStartTime = std::chrono::steady_clock::now(),
            .animator = std::make_shared<Animator>(m_playerAnims["DanIdle"])
        };
        m_players[playerNumber].currentAnim = BIRTH;

        pos = bufferView.find("pnw", pos);
    }


    // Player position update (ppo playerNumber x y orientation)
    PARSER_FIRST_SYMBOL("ppo");
    while (pos != std::string::npos) {
        const int playerNumber = PARSER_NEXT_INT(bufferView, pos);

        if (m_players.find(playerNumber) == m_players.end())
            throw std::runtime_error("ppo: player [" + std::to_string(playerNumber) + "] not found");

        {   // Get the new position
            glm::ivec2 position;
            position[0] = PARSER_NEXT_INT(bufferView, pos);
            position[1] = PARSER_NEXT_INT(bufferView, pos);
            position = (position - m_mapSize / 2);

            // Add to to the logs if the player moved
            glm::vec3 oldPos = m_players[playerNumber].position / glm::vec3(m_tileSpacing[0] + m_tileSize[0], 0, m_tileSpacing[1] + m_tileSize[1]);
            glm::ivec2 oldPosI = {static_cast<int>(oldPos[0]), static_cast<int>(oldPos[2])};
            if (oldPosI != position) {
                LOG("Player [" + std::to_string(playerNumber) + "] moved forward", BLUE);
                m_players[playerNumber].currentAnim = MOVE;
                m_players[playerNumber].moveOrientation = glm::vec3(static_cast<float>(position[0]) * (m_tileSpacing[0] + m_tileSize[0]), m_playerHeight, static_cast<float>(position[1]) * (m_tileSpacing[1] +  + m_tileSize[1])) - m_players[playerNumber].position;
            }

            m_players[playerNumber].position = glm::vec3(static_cast<float>(position[0]) * (m_tileSpacing[0] + m_tileSize[0]), m_playerHeight, static_cast<float>(position[1]) * (m_tileSpacing[1] +  + m_tileSize[1]));
        }


        {   // Get the new orientation
            const int newOrientation = PARSER_LAST_INT(bufferView, pos);

            // Add to the logs if the player changed orientation
            const int oldOrientation = m_players[playerNumber].orientation;
            if (oldOrientation != newOrientation) {
                if (newOrientation == (oldOrientation + 1) % 4)
                    LOG("Player [" + std::to_string(playerNumber) + "] turned right", BLUE);
                else if (newOrientation == (oldOrientation + 3) % 4)
                    LOG("Player [" + std::to_string(playerNumber) + "] turned left", BLUE);
            }

            m_players[playerNumber].orientation = newOrientation;
        }


        // Get the next player
        PARSER_NEXT_SYMBOL("ppo");
    }


    // Player broadcast (pbc playerNumber message)
    PARSER_FIRST_SYMBOL("pbc");
    while (pos != std::string::npos) {
        const int playerNumber = PARSER_NEXT_INT(bufferView, pos);
        const std::string message = PARSER_LAST_STRING(bufferView, pos);

        if (m_players.find(playerNumber) == m_players.end())
            throw std::runtime_error("pbc: player [" + std::to_string(playerNumber) + "] not found");

        // Clear the broadcast at the player position & playerNumber
        m_broadcasts.erase(std::remove_if(m_broadcasts.begin(), m_broadcasts.end(), [playerNumber, this](const Broadcast& broadcast) {
            return broadcast.position == m_players[playerNumber].position || broadcast.playerID == playerNumber;
        }), m_broadcasts.end());

        m_broadcasts.push_back(Broadcast {
            .startTime = std::chrono::high_resolution_clock::now(),
            .playerID = playerNumber,
            .position = m_players[playerNumber].position
        });

        LOG("Player [" + std::to_string(playerNumber) + "] broadcasted: " + message, GREEN);

        PARSER_NEXT_SYMBOL("pbc");
    }


    // Player incantation (pic X Y level playerNumber playerNumber ...)
    PARSER_FIRST_SYMBOL("pic");
    while (pos != std::string::npos) {
        const int x = PARSER_NEXT_INT(bufferView, pos);
        const int y = PARSER_NEXT_INT(bufferView, pos);
        const int level = PARSER_NEXT_INT(bufferView, pos);
        pos = bufferView.find(' ', pos) + 1;

        std::vector<int> playerNumbers;
        while (bufferView[pos] != '\n') {
            playerNumbers.push_back(std::stoi(bufferView.substr(pos, bufferView.find(' ', pos) - pos)));

            if (m_players.find(playerNumbers.back()) == m_players.end())
                throw std::runtime_error("pic: player [" + std::to_string(playerNumbers.back()) + "] not found");

            size_t nextSpace = bufferView.find(' ', pos);
            if (nextSpace == std::string::npos)
                break;

            size_t nextNewLine = bufferView.find('\n', pos);
            pos = (nextSpace < nextNewLine) ? nextSpace + 1 : nextNewLine;
        }

        std::string players;
        for (const auto& playerNumber : playerNumbers) {
            players += std::to_string(playerNumber) + " ";
            m_players[playerNumber].currentAnim = RITUAL;
        }
        LOG("Incantation started to level " + std::to_string(level) + " at position [" + std::to_string(x) + ", " + std::to_string(y) + "] with players: " + players, GREEN);

        // TODO: use this datas
        (void) x;
        (void) y;
        (void) level;
        (void) players;

        PARSER_NEXT_SYMBOL("pic");

    }


    // Player incantation end (pie X Y result)
    PARSER_FIRST_SYMBOL("pie");
    while (pos != std::string::npos) {
        const int x = PARSER_NEXT_INT(bufferView, pos);
        const int y = PARSER_NEXT_INT(bufferView, pos);
        const int result = PARSER_LAST_INT(bufferView, pos);

        LOG("Incantation ended: " + std::to_string(result) + " at [" + std::to_string(x) + ", " + std::to_string(y) + "]", GREEN);

        // TODO: use this datas
        (void) x;
        (void) y;
        (void) result;

        PARSER_NEXT_SYMBOL("pie");
    }

    // Player inventory (pin playerNumber x y q0 q1 q2 q3 q4 q5 q6)
    PARSER_FIRST_SYMBOL("pin");
    while (pos != std::string::npos)
    {
        const int id = PARSER_NEXT_INT(bufferView, pos);
        [[maybe_unused]] const int x = PARSER_NEXT_INT(bufferView, pos);
        [[maybe_unused]] const int y = PARSER_NEXT_INT(bufferView, pos);
        for (std::size_t i = 0; i < RESNUMBER - 1; ++i)
        {
            m_players.at(id).inv.ressources[i] = PARSER_NEXT_INT(bufferView, pos);
        }
        m_players.at(id).inv.ressources[RESNUMBER - 1] = PARSER_LAST_INT(bufferView, pos);
        PARSER_NEXT_SYMBOL("pin");
    }


    // Player level (plv playerNumber level)
    PARSER_FIRST_SYMBOL("plv");
    while (pos != std::string::npos)
    {
        const int id = PARSER_NEXT_INT(bufferView, pos);
        const int lvl = PARSER_LAST_INT(bufferView, pos);
        m_players.at(id).level = lvl;
        PARSER_NEXT_SYMBOL("plv");
    }


    // Player death (pdi playerNumber)
    PARSER_FIRST_SYMBOL("pdi");
    while (pos != std::string::npos) {
        const int playerNumber = PARSER_LAST_INT(bufferView, pos);

        if (m_players.find(playerNumber) == m_players.end())
            throw std::runtime_error("pdi: player [" + std::to_string(playerNumber) + "] not found");

        // Add to the logs
        LOG("Player [" + std::to_string(playerNumber) + "] died", RED);

        m_players.erase(playerNumber);
        PARSER_NEXT_SYMBOL("pdi");
    }

    PARSER_NEXT_SYMBOL("pex");
    while (pos != std::string::npos) {
        const int playerNumber = PARSER_LAST_INT(bufferView, pos);
        if (m_players.find(playerNumber) == m_players.end())
            throw std::runtime_error("pex: player [" + std::to_string(playerNumber) + "] not found");
        LOG("Player [" + std::to_string(playerNumber) + "] expulsed", GREEN);
        PARSER_NEXT_SYMBOL("pex");
    }

    PARSER_NEXT_SYMBOL("pfk");
    while (pos != std::string::npos) {
        const int playerNumber = PARSER_LAST_INT(bufferView, pos);
        if (m_players.find(playerNumber) == m_players.end())
            throw std::runtime_error("pfk: player [" + std::to_string(playerNumber) + "] not found");
        LOG("Player [" + std::to_string(playerNumber) + "] just layed an egg !", BLUE);
        PARSER_NEXT_SYMBOL("pfk");
    }
}

void App::parseConnectionResponse() {
    std::array<char, BUFFER_SIZE> buffer{};
    buffer.fill(0);

    // Wait for the reply
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(_networkManager.getSocket(), &readfds);
    timeval timeout { .tv_sec = 2, .tv_usec = 0 };
    if (select(_networkManager.getSocket()+ 1, &readfds, nullptr, nullptr, &timeout) > 0) {
        // Wait for the multiple welcome messages to stack
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        size_t readSize = 0;
        while (readSize < BUFFER_SIZE) {
            readSize += read(_networkManager.getSocket(), buffer.data() + readSize, BUFFER_SIZE - readSize);
            if (buffer[readSize - 1] == '\n')
                break;
        }
    } else {
        throw std::runtime_error("No response from the server");
    }

    const std::string& bufferView(buffer.data());


    // Get the team names (tna teamName\n * n)
    PARSER_INIT();
    PARSER_FIRST_SYMBOL("tna");
    if (pos == std::string::npos)
        throw std::runtime_error("tna not found in welcome bufferView");

    while (pos != std::string::npos) {
        const std::string teamName = PARSER_LAST_STRING(bufferView, pos);

        m_teams[teamName].mesh.first = "Dan";
        m_teams[teamName].mesh.second = m_playerMeshes[m_teams[teamName].mesh.first].first;
        std::srand(std::rand() * std::time(nullptr));
        float r = static_cast<float>(std::rand()) / RAND_MAX;
        float g = static_cast<float>(std::rand()) / RAND_MAX;
        float b = static_cast<float>(std::rand()) / RAND_MAX;
        m_teams[teamName].teamColor = glm::vec3(r, g, b);
        PARSER_NEXT_SYMBOL("tna");
    }


    // Get the speed of the server (sgt speed\n)
    PARSER_FIRST_SYMBOL("sgt");
    if (pos == std::string::npos)
        throw std::runtime_error("sgt not found in welcome bufferView");

    m_speed = PARSER_LAST_INT(bufferView, pos);

    m_mapSize = parseMapSize(bufferView);
    m_map.resize(m_mapSize[0], std::vector<TileContent>(m_mapSize[1]));
    updateMap(bufferView);
    updatePlayers(bufferView);
    updateEggs(bufferView);
}

void App::updateEggs(const std::string& bufferView) {
    PARSER_INIT();

    // A new egg is "enw eggNumber playerId x y\n"
    PARSER_FIRST_SYMBOL("enw");
    while (pos != std::string::npos) {
        glm::ivec2 position;

        const int eggNumber = PARSER_NEXT_INT(bufferView, pos);
        const int playerId = PARSER_NEXT_INT(bufferView, pos);
        position[0] = PARSER_NEXT_INT(bufferView, pos);
        position[1] = PARSER_LAST_INT(bufferView, pos);
        position = (position - m_mapSize / 2);

        m_eggs[eggNumber] = Egg{
            .position = glm::vec3(static_cast<float>(position[0]) * (m_tileSpacing[0] + m_tileSize[0]), m_playerHeight, static_cast<float>(position[1]) * (m_tileSpacing[1] + m_tileSize[1]))
        };

        LOG("Egg [" + std::to_string(eggNumber) + "] was laid by player [" + std::to_string(playerId) + "] at position [" + std::to_string(position[0]) + ", " + std::to_string(position[1]) + "]", YELLOW);

        PARSER_NEXT_SYMBOL("enw");
    }


    // Egg dying (edi eggNumber)
    PARSER_FIRST_SYMBOL("edi");
    while (pos != std::string::npos) {
        const int eggNumber = PARSER_LAST_INT(bufferView, pos);

        m_eggs.erase(eggNumber);
        LOG("Egg [" + std::to_string(eggNumber) + "] died", YELLOW);

        PARSER_NEXT_SYMBOL("edi");
    }


    // Player born from an egg (ebo eggNumber)
    PARSER_FIRST_SYMBOL("ebo");
    while (pos != std::string::npos) {
        const int eggNumber = PARSER_LAST_INT(bufferView, pos);

        m_eggs.erase(eggNumber);
        LOG("Egg [" + std::to_string(eggNumber) + "] gave birth to a player", YELLOW);

        PARSER_NEXT_SYMBOL("ebo");
    }
}

void App::updateMap(const std::string& bufferView) {
    // Set the ressources on the map (bct x y q0 q1 q2 q3 q4 q5 q6)
    PARSER_INIT();
    PARSER_FIRST_SYMBOL("bct");
    while (pos != std::string::npos) {
        // Get the position of the tile
        int x = PARSER_NEXT_INT(bufferView, pos);
        int y = PARSER_NEXT_INT(bufferView, pos);

        // Get the ressources on the tile
        for (int i = 0; i < 6; i++) {
            m_map[x][y].ressources[i] = PARSER_NEXT_INT(bufferView, pos);
        }
        m_map[x][y].ressources[6] = PARSER_LAST_INT(bufferView, pos);

        PARSER_NEXT_SYMBOL("bct");
    }
    _mapInventory.ressources.fill(0);
    for (int x = 0; x < m_mapSize[0]; ++x)
        for (int y = 0; y < m_mapSize[1]; ++y)
            for (std::size_t i = 0; i < RESNUMBER; ++i)
                _mapInventory.ressources[i] += m_map[x][y].ressources[i];
}

glm::ivec2 App::parseMapSize(const std::string& bufferView) {
    // Get the map size (msz x y\n)
    PARSER_INIT();
    PARSER_FIRST_SYMBOL("msz");
    if (pos == std::string::npos)
        throw std::runtime_error("msz not found in welcome buffer");

    glm::ivec2 mapSize;
    mapSize[0] = PARSER_NEXT_INT(bufferView, pos);
    mapSize[1] = PARSER_LAST_INT(bufferView, pos);

    return mapSize;
}
