/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** ai_clients
*/

#include "commmands/commands.h"

#include <stdlib.h>
#include <string.h>

#include "ai_internal.h"
#include "server.h"

// Here we define the commands in a static array
// so we can get the size at compile time.
// It needs to be sorted for the bsearch function.
static const struct ai_cmd_s commands[] = {
    {"Broadcast", ai_cmd_broadcast, 7},
    {"Connect_nbr", NULL, 0},
    {"Eject", ai_cmd_eject, 7},
    {"Fork", NULL, 42},
    {"Forward", ai_cmd_forward, 7},
    {"Incantation", NULL, 300},
    {"Inventory", ai_cmd_inventory, 1},
    {"Left", ai_cmd_left, 7},
    {"Look", ai_cmd_look, 7},
    {"Right", ai_cmd_right, 7},
    {"Set", ai_cmd_set, 7},
    {"Take", ai_cmd_take, 7},
};

const struct ai_cmd_s *const AI_CLIENT_COMMANDS = commands;
const size_t AI_CLIENT_COMMANDS_LEN = LEN(commands);

static int compare(const char *buffer, const struct ai_cmd_s *cmd)
{
    if (buffer == NULL || cmd == NULL || cmd->cmd == NULL)
        return 1;
    return strcmp(buffer, cmd->cmd);
}

struct ai_cmd_s *get_ai_cmd(const char *cmd)
{
    return bsearch(
        cmd, AI_CLIENT_COMMANDS, AI_CLIENT_COMMANDS_LEN,
        sizeof *AI_CLIENT_COMMANDS,
        (int (*)(const void *, const void *))compare);
}
