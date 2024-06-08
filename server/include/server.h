/*
** EPITECH PROJECT, 2024
** include
** File description:
** server
*/

#pragma once

#include "array.h"

#include <bits/stdint-uintn.h>
#include <unistd.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define ATTR(x) __attribute__((x))
#define UNUSED ATTR(unused)
#define LEN(x) (sizeof(x) / sizeof*(x))
#define IDX(x, y, w, h) ((((y) % (h) + (h)) % (h)) * (w) + (((x) % (w) + (w)) % (w)))

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
    int x;
    int y;
} vector_t;

extern const double DENSITIES[7];

struct cell_s {
    vector_t pos;
    ressource_t res[7];
};

typedef struct cell_s cell_t;

typedef struct payload_s {
    ressource_t res[7];
} payload_t;

typedef struct look_payload_s {
    size_t size;
    size_t idx;
    payload_t *cell_content;
} look_payload_t;

typedef struct context_s {
    int port;
    size_t width;
    size_t height;
    size_t map_size;
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
    struct sockaddr_in s_addr;
    array_t ai_clients;
    array_t waitlist_fd;
    cell_t *map;
} server_t;

int server(UNUSED int argc, UNUSED char **argv);
payload_t *get_cell_payload(server_t *server, context_t *ctx, vector_t *pos, payload_t *payload);
look_payload_t *look(server_t *server, context_t *ctx, ai_client_t *client);
