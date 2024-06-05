/*
** EPITECH PROJECT, 2024
** src
** File description:
** server
*/

#include "server.h"

const ressource_t r_index[] = {
    {FOOD, 0.5},
    {LINEMATE, 0.3},
    {DERAUMERE, 0.15},
    {SIBUR, 0.1},
    {MENDIANE, 0.1},
    {PHIRAS, 0.08},
    {THYSTAME, 0.05}
};

const struct cell_s default_cell = {
    .pos = {0},
    .res[0] = {FOOD, 0},
    .res[1] = {LINEMATE, 0},
    .res[2] = {DERAUMERE, 0},
    .res[3] = {SIBUR, 0},
    .res[4] = {MENDIANE, 0},
    .res[5] = {PHIRAS, 0},
    .res[6] = {THYSTAME, 0},
};

int server(UNUSED int argc, UNUSED char const *argv[])
{
    return RET_VALID;
}
