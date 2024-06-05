/*
** EPITECH PROJECT, 2024
** include
** File description:
** server
*/

#pragma once

#include "array.h"

#include <unistd.h>
#include <stdbool.h>
#include <uuid/uuid.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define ATTR(x) __attribute__((x))
#define UNUSED ATTR(unused)
#define LEN(x) (sizeof(x) / sizeof*(x))
#define IDX(x, y, w, h) (((y) % (h)) * (w) + ((x) % (w)))

enum {
    RET_VALID = 0,
    RET_ERROR = 84,
};

typedef enum {
    FOOD,
    LINEMATE,
    DERAUMERE,
    SIBUR,
    MENDIANE,
    PHIRAS,
    THYSTAME
} res_name_t;

typedef struct {
    res_name_t r_name;
    int quantity;
} ressource_t;

typedef struct {
    size_t x;
    size_t y;
} vector_t;

extern const double DENSITIES[];

struct cell_s {
    vector_t pos;
    ressource_t res[7];
};

typedef struct cell_s cell_t;

typedef struct context_s {
    int port;
    size_t width;
    size_t height;
    array_t *names;
    size_t client_nb;
    double freq;
} context_t;

typedef struct ai_client_s {
    int s_fd;
    char team[512];
    ressource_t res[7];
    vector_t pos;
    enum {
        NORTH,
        EAST,
        SOUTH,
        WEST
    } dir;
    int lvl;
} ai_client_t;

typedef struct server_s {
    int s_fd;
    array_t *ai_clients;
    cell_t *map;
} server_t;

int server(UNUSED int argc, UNUSED char **argv);
