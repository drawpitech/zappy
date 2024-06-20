/*
** EPITECH PROJECT, 2024
** commands
** File description:
** gui_cmd_pnw
*/

#include <stdio.h>

#include "commands.h"
#include "server.h"

void gui_cmd_pnw(server_t *server, gui_client_t *client, ai_client_t *new)
{
    if (client == NULL)
        return;
    dprintf(
        client->s_fd, "#%d %d %d %d %d %s\n", new->id, new->pos.x, new->pos.y,
        new->dir, new->lvl, new->team);
}
