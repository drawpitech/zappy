/*
** EPITECH PROJECT, 2024
** commands
** File description:
** gui_cmd_sgt
*/

#include <stdio.h>
#include <stdlib.h>

#include "commands.h"

void gui_cmd_sgt(server_t *server, gui_client_t *client, char *args)
{
    dprintf(client->s_fd, "sgt %f\n", server->ctx.freq);
}
