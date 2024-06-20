/*
** EPITECH PROJECT, 2024
** commands
** File description:
** gui_cmd_suc
*/

#include <stdio.h>
#include <stdlib.h>

#include "commands.h"
#include "server.h"

void gui_cmd_suc(UNUSED server_t *server, gui_client_t *client)
{
    if (client == NULL)
        return;
    gui_dprintf(client, "suc\n");
}
