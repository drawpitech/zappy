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
#define MOD(x, y) ((((x) % (y)) + (y)) % (y))
#define IDX(x, y, w, h) (MOD(y, h) * (w) + MOD(x, w))
#define R_COUNT 9
#define CELL(s, x, y) (&s->map[IDX(x, y, s->ctx.width, s->ctx.height)])

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
    THYSTAME,
    EGG,
    PLAYER,
} res_name_t;

typedef enum {
    S_EAST,
    S_NORTH_EAST,
    S_NORTH,
    S_NORTH_WEST,
    S_WEST,
    S_SOUTH_WEST,
    S_SOUTH,
    S_SOUTH_EAST
} sound_direction_t;

typedef struct {
    res_name_t r_name;
    int quantity;
} ressource_t;

typedef struct {
    int x;
    int y;
} vector_t;

extern const double DENSITIES[R_COUNT];
extern const char *const r_name[R_COUNT];

struct cell_s {
    vector_t pos;
    ressource_t res[R_COUNT];
};

typedef struct cell_s cell_t;

typedef struct payload_s {
    ressource_t res[R_COUNT];
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
    ressource_t res[R_COUNT];
    vector_t pos;
    enum {
        NORTH,
        EAST,
        SOUTH,
        WEST
    } dir;
    int lvl;
    char buffer[4096];
} ai_client_t;

typedef struct server_s {
    int s_fd;
    struct sockaddr_in s_addr;
    array_t ai_clients;
    array_t waitlist_fd;
    cell_t *map;
    context_t ctx;
} server_t;

int server(UNUSED int argc, UNUSED char **argv);
payload_t *get_cell_payload(server_t *serv, vector_t *pos, payload_t *payload);
res_name_t get_ressource_type(char *name);

int iterate_ai_clients(server_t *server);
int init_ai_client(server_t *server, int client_fd, char *team);
int remove_ai_client(server_t *server, size_t idx);
void move_ai_client(server_t *server, ai_client_t *client, int dir);
