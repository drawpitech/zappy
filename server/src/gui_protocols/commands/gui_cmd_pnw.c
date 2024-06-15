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
    size_t client_nb = server->ai_clients.size;
    ai_client_t *last_client = server->ai_clients.elements[client_nb];

    dprintf(
        client->s_fd, "#%ld %d %d %d %d %s\n", client_nb, last_client->pos.x,
        last_client->pos.y, last_client->dir, last_client->lvl,
        last_client->team);
}
