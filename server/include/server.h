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

const static ressource_t r_index[] = {
    {FOOD, 0.5},
    {LINEMATE, 0.3},
    {DERAUMERE, 0.15},
    {SIBUR, 0.1},
    {MENDIANE, 0.1},
    {PHIRAS, 0.08},
    {THYSTAME, 0.05}
};

typedef struct {
    vector_t pos;
} cell_t;

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
