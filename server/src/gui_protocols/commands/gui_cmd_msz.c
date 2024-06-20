/*
** EPITECH PROJECT, 2024
** commands
** File description:
** gui_cmd_msz
*/

#include <stdio.h>

#include "commands.h"
#include "server.h"

void gui_cmd_msz(server_t *server, gui_client_t *client, UNUSED char *args)
{
    if (client == NULL)
        return;
    gui_dprintf(
        client, "msz %ld %ld\n", server->ctx.width, server->ctx.height);
}
