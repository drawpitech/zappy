/*
** EPITECH PROJECT, 2024
** commands
** File description:
** gui_cmd_sgt
*/

#include <stdio.h>
#include <stdlib.h>

#include "commands.h"

void gui_cmd_sgt(server_t *server, gui_client_t *client, UNUSED char *args)
{
    if (client == NULL)
        return;
    gui_dprintf(client, "sgt %ld\n", server->ctx.freq);
}
