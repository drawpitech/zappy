/*
** EPITECH PROJECT, 2024
** commands
** File description:
** gui_cmd_pnw
*/

#include <stdio.h>

#include "commands.h"
#include "server.h"

void gui_cmd_pnw(server_t *server, gui_client_t *client, UNUSED char *args)
{
    ai_client_t *last_client = NULL;

    if (client == NULL || server->ai_clients.size == 0)
        return;
    last_client = server->ai_clients.elements[server->ai_clients.size - 1];
    dprintf(
        client->s_fd, "#%ld %d %d %d %d %s\n", server->ai_clients.size,
        last_client->pos.x, last_client->pos.y, last_client->dir,
        last_client->lvl, last_client->team);
}
