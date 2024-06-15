/*
** EPITECH PROJECT, 2024
** commands
** File description:
** gui_cmd_plv
*/

#include <stdio.h>
#include <stdlib.h>

#include "commands.h"
#include "server.h"

void gui_cmd_pin(server_t *server, gui_client_t *client, char *args)
{
    char **cmd = my_str_to_word_array(args, " \n#");
    int client_idx = atoi(*cmd);
    ai_client_t *current = NULL;

    if ((size_t)client_idx > server->ai_clients.size) {
        dprintf(client->s_fd, "ko\n");
        return;
    }
    current = server->ai_clients.elements[client_idx];
    dprintf(
        client->s_fd, "#%d %d %d %d %d %d %d %d %d %d\n", client_idx,
        current->pos.x, current->pos.y, current->res[FOOD].quantity,
        current->res[LINEMATE].quantity, current->res[DERAUMERE].quantity,
        current->res[SIBUR].quantity, current->res[MENDIANE].quantity,
        current->res[PHIRAS].quantity, current->res[THYSTAME].quantity);
}
