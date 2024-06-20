/*
** EPITECH PROJECT, 2024
** commands
** File description:
** gui_cmd_pfk
*/

#include <stdio.h>
#include <stdlib.h>

#include "commands.h"

void gui_cmd_pfk(UNUSED server_t *server, gui_client_t *client, int ai_id)
{
    if (client == NULL)
        return;
    dprintf(client->s_fd, "pfk %d\n", ai_id);
}
