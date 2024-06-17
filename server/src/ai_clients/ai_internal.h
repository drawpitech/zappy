/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** ia_internal
*/

#pragma once

#include "server.h"

typedef void (*ai_cmd_func_t)(server_t *, ai_client_t *, char *);

struct ai_cmd_s {
    char *cmd;
    ai_cmd_func_t func;
    size_t time;
};

typedef struct {
    ai_cmd_func_t func;
    char *args;
    size_t time;
} queued_cmd_t;


extern const struct ai_cmd_s *const AI_CLIENT_COMMANDS;
extern const size_t AI_CLIENT_COMMANDS_LEN;

void disconnect_ai_client(ai_client_t *ai);
void handle_ai_client(server_t *server, ai_client_t *client);
