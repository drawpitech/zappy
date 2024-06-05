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
    double density;
} ressource_t;

typedef struct {
    size_t x;
    size_t y;
} vector_t;

extern const ressource_t r_index[];

struct cell_s {
    vector_t pos;
    ressource_t res[7];
} extern const default_cell;

typedef struct cell_s cell_t;

typedef struct context_s {
    int port;
    size_t width;
    size_t height;
    char **names;
    size_t client_nb;
    double freq;
} context_t;

typedef struct server_s {
    int s_fd;
    struct sockaddr_in s_addr;
    array_t *clients;
} server_t;

int server(UNUSED int argc, UNUSED char const *argv[]);
