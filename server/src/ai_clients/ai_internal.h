/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** ia_internal
*/

#pragma once

#include "server.h"

typedef struct {
    server_t *server;
    struct context_s *ctx;
    size_t ai_idx;
    ai_client_t *ai;
} ai_cmd_ctx_t;

typedef void (*ai_cmd_func_t)(ai_cmd_ctx_t *, char *);

struct ai_cmd_s {
    char *cmd;
    ai_cmd_func_t func;
    size_t time;
};

extern const struct ai_cmd_s *const AI_CLIENT_COMMANDS;
extern const size_t AI_CLIENT_COMMANDS_LEN;

struct ai_cmd_s *get_ai_cmd(const char *cmd);
