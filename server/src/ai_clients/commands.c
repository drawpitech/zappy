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

// TODO:
// - There will be some big issues if a users tries to send > 4096 bytes
// without a \n.

const struct ai_cmd_s *const AI_CLIENT_COMMANDS = commands;
const size_t AI_CLIENT_COMMANDS_LEN = LEN(commands);

static int compare(const char *buffer, const struct ai_cmd_s *cmd)
{
    if (buffer == NULL || cmd == NULL || cmd->cmd == NULL)
        return 1;
    return strcmp(buffer, cmd->cmd);
}

static struct ai_cmd_s *get_ai_cmd(const char *cmd)
{
    return bsearch(
        cmd, AI_CLIENT_COMMANDS, AI_CLIENT_COMMANDS_LEN,
        sizeof *AI_CLIENT_COMMANDS,
        (int (*)(const void *, const void *))compare);
}

static void exec_ai_cmd(server_t *server, ai_client_t *client, char *buffer)
{
    char *content = NULL;
    struct ai_cmd_s *cmd = NULL;

    content = strchr(buffer, ' ');
    if (content != NULL) {
        *content = '\0';
        content++;
    } else {
        content = "";
    }
    cmd = get_ai_cmd(buffer);
    if (cmd == NULL || cmd->func == NULL) {
        write(client->s_fd, "ko\n", 3);
        return;
    }
    cmd->func(server, client, content);
}

static bool process_ai_cmd(server_t *server, ai_client_t *client, char **ptr)
{
    char *buffer = *ptr;
    char *newline = strchr(buffer, '\n');

    if (newline == NULL) {
        strcat(client->buffer, buffer);
        return false;
    }
    *newline = '\0';
    if (buffer != newline && *(newline - 1) == '\r')
        *(newline - 1) = '\0';
    exec_ai_cmd(server, client, buffer);
    *ptr = newline + 1;
    return true;
}

void handle_ai_client(server_t *server, ai_client_t *client)
{
    char buffer[sizeof client->buffer];
    ssize_t bytes_read = 0;
    size_t offset = 0;

    strcpy(buffer, client->buffer);
    offset = strlen(buffer);
    client->buffer[0] = '\0';
    bytes_read = read(client->s_fd, buffer + offset, sizeof buffer - offset);
    if (bytes_read <= 0) {
        disconnect_ai_client(client);
        return;
    }
    buffer[bytes_read] = '\0';
    for (char *ptr = buffer; process_ai_cmd(server, client, &ptr);)
        ;
}
