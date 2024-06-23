/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** This file is so cursed
*/

#include "App.hpp"

#include "Utils.hpp"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "glm/common.hpp"
#include "glm/ext/vector_float3.hpp"
#include "imgui.h"

void App::moveAnimation(Player &player) {
    if (player.moveOrientation.x + player.moveOrientation.z > m_tileSize[0] * 2 or player.moveOrientation.x + player.moveOrientation.z < -m_tileSize[0] * 2) {
        player.visualPositionOffset = glm::vec3(0, 0, 0);
        player.currentAction = IDLE;
        player.currentAnim = IDLE;
        createPlayers();
    }
    glm::vec3 tempOffset;
    if (player.moveOrientation[0] > 0) {
        if (player.visualPositionOffset[0] < 0)
            tempOffset = {(m_tileSpacing[0] + m_tileSize[0]) * m_moveSpeed * m_speed * m_frameTime, 0, 0}; // NOLINT
        else {
            player.visualPositionOffset = {0, 0, 0};
            player.currentAction = IDLE;
            player.currentAnim = IDLE;
        }
    }
    if (player.moveOrientation[0] < 0) {
        if (player.visualPositionOffset[0] > 0)
            tempOffset = {-(m_tileSpacing[0] + m_tileSize[0]) * m_moveSpeed * m_speed * m_frameTime, 0, 0}; // NOLINT
        else {
            player.visualPositionOffset = {0, 0, 0};
            player.currentAction = IDLE;
            player.currentAnim = IDLE;
        }
    }

    if (player.moveOrientation[2] > 0) {
        if (player.visualPositionOffset[2] < 0)
            tempOffset = {0, 0, (m_tileSpacing[1] + m_tileSize[2]) * m_moveSpeed * m_speed * m_frameTime}; // NOLINT
        else {
            player.visualPositionOffset = {0, 0, 0};
            player.currentAction = IDLE;
            player.currentAnim = IDLE;
        }
    }
    if (player.moveOrientation[2] < 0) {
        if (player.visualPositionOffset[2] > 0)
            tempOffset = {0, 0, -(m_tileSpacing[1] + m_tileSize[2]) * m_moveSpeed * m_speed * m_frameTime}; // NOLINT
        else {
            player.visualPositionOffset = {0, 0, 0};
            player.currentAction = IDLE;
            player.currentAnim = IDLE;
        }
    }
    player.visualPositionOffset += tempOffset;
    if (tempOffset.x + tempOffset.z > m_tileSize[0] * 2 or tempOffset.x + tempOffset.z < -m_tileSize[0] * 2) {
        player.visualPositionOffset = {0, 0, 0};
        player.currentAction = IDLE;
        player.currentAnim = IDLE;
    }
    createPlayers();
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
            player.currentAction = RITUAL;
            player.visualPositionOffset = glm::vec3(0, 0, 0);
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
            moveAnimation(player);
        } else {
            player.visualPositionOffset = glm::vec3(0, 0, 0);
        }
    }
}