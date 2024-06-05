/*
** EPITECH PROJECT, 2024
** src
** File description:
** server
*/

#include "server.h"

const double DENSITIES[] = {
    [FOOD] = 0.5,
    [LINEMATE] = 0.3,
    [DERAUMERE] = 0.15,
    [SIBUR] = 0.1,
    [MENDIANE] = 0.1,
    [PHIRAS] = 0.08,
    [THYSTAME] = 0.05
};

const struct cell_s DEFAULT_CELL = {
    .pos = {0},
    .res[0] = {FOOD, 0},
    .res[1] = {LINEMATE, 0},
    .res[2] = {DERAUMERE, 0},
    .res[3] = {SIBUR, 0},
    .res[4] = {MENDIANE, 0},
    .res[5] = {PHIRAS, 0},
    .res[6] = {THYSTAME, 0},
};

int server(UNUSED int argc, UNUSED char **argv)
{
    return RET_VALID;
}
