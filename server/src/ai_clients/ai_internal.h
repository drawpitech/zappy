/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** ia_internal
*/

#pragma once

#include "server.h"

struct ai_cmd_s{
    const char *cmd;
    void (*const func)(server_t *, size_t, ai_client_t *, char *);
};

extern const struct ai_cmd_s *const AI_CLIENT_COMMANDS;
extern const size_t AI_CLIENT_COMMANDS_LEN;

struct ai_cmd_s *get_ai_cmd(const char *cmd);
