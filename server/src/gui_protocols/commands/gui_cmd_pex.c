/*
** EPITECH PROJECT, 2024
** commands
** File description:
** gui_cmd_pex
*/

#include <stdio.h>
#include <stdlib.h>

#include "commands.h"

void gui_cmd_pex(UNUSED server_t *server, gui_client_t *client, int ai_id)
{
    if (client == NULL)
        return;
    gui_dprintf(client, "pex %d\n", ai_id);
}
