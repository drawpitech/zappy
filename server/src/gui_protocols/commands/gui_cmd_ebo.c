/*
** EPITECH PROJECT, 2024
** commands
** File description:
** gui_cmd_ebo
*/

#include <stdio.h>
#include <stdlib.h>

#include "commands.h"
#include "server.h"

void gui_cmd_ebo(UNUSED server_t *server, gui_client_t *client, egg_t *egg)
{
    if (client == NULL)
        return;
    gui_dprintf(client, "ebo %d\n", egg->id);
}
