/*
** EPITECH PROJECT, 2024
** src
** File description:
** incantations
*/

#include "incantation.h"
#include "server.h"

const payload_t INC_NEEDS[7] = {
    {
        {{FOOD, 0}, {LINEMATE, 1}, {DERAUMERE, 0}, {SIBUR, 0},
            {MENDIANE, 0}, {PHIRAS, 0}, {THYSTAME, 0}, {EGG, 0}, {PLAYER, 1}}
    },
    {
        {{FOOD, 0}, {LINEMATE, 1}, {DERAUMERE, 1}, {SIBUR, 1},
            {MENDIANE, 0}, {PHIRAS, 2}, {THYSTAME, 0}, {EGG, 0}, {PLAYER, 2}}
    },
    {
        {{FOOD, 0}, {LINEMATE, 2}, {DERAUMERE, 0}, {SIBUR, 1},
            {MENDIANE, 0}, {PHIRAS, 2}, {THYSTAME, 0}, {EGG, 0}, {PLAYER, 2}}
    },
    {
        {{FOOD, 0}, {LINEMATE, 1}, {DERAUMERE, 1}, {SIBUR, 2},
            {MENDIANE, 0}, {PHIRAS, 1}, {THYSTAME, 0}, {EGG, 0}, {PLAYER, 4}}
    },
    {
        {{FOOD, 0}, {LINEMATE, 1}, {DERAUMERE, 2}, {SIBUR, 2},
            {MENDIANE, 3}, {PHIRAS, 0}, {THYSTAME, 0}, {EGG, 0}, {PLAYER, 4}}
    },
    {
        {{FOOD, 0}, {LINEMATE, 1}, {DERAUMERE, 2}, {SIBUR, 3},
            {MENDIANE, 0}, {PHIRAS, 1}, {THYSTAME, 0}, {EGG, 0}, {PLAYER, 6}}
    },
    {
        {{FOOD, 0}, {LINEMATE, 2}, {DERAUMERE, 2}, {SIBUR, 2},
            {MENDIANE, 2}, {PHIRAS, 1}, {THYSTAME, 0}, {EGG, 0}, {PLAYER, 6}}
    },
};
