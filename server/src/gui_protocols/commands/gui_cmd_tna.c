/*
** EPITECH PROJECT, 2024
** commands
** File description:
** gui_cmd_tna
*/

#include <stdio.h>

#include "commands.h"

void gui_cmd_tna(server_t *server, gui_client_t *client, char *args)
{
    for (size_t i = 0; i < server->ctx.names->size; ++i)
        dprintf(
            client->s_fd, "tna %s\n", (char *)server->ctx.names->elements[i]);
}
