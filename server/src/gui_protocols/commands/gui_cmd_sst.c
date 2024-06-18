/*
** EPITECH PROJECT, 2024
** commands
** File description:
** gui_cmd_sst
*/

#include <stdio.h>
#include <stdlib.h>

#include "commands.h"

void gui_cmd_sst(server_t *server, gui_client_t *client, char *args)
{
    long freq = atol(args);

    if (client == NULL)
        return;
    if (freq == 0) {
        dprintf(client->s_fd, "ko\n");
        return;
    }
    server->ctx.freq = freq;
    dprintf(client->s_fd, "sst %ld\n", freq);
}
