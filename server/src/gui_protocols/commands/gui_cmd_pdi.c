/*
** EPITECH PROJECT, 2024
** commands
** File description:
** gui_cmd_pdi
*/

#include <stdio.h>
#include <stdlib.h>

#include "commands.h"
#include "server.h"

void gui_cmd_pdi(UNUSED server_t *server, gui_client_t *client, int ai_id)
{
    if (client == NULL)
        return;
    dprintf(client->s_fd, "pdi #%d\n", ai_id);
}
