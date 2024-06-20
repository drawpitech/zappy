/*
** EPITECH PROJECT, 2024
** commands
** File description:
** gui_cmd_pgt
*/

#include <stdio.h>
#include <stdlib.h>

#include "commands.h"

void
gui_cmd_pgt(UNUSED server_t *server, gui_client_t *client, int ai_id, int r)
{
    if (client == NULL)
        return;
    dprintf(client->s_fd, "pdr %d %d\n", ai_id, r);
}
