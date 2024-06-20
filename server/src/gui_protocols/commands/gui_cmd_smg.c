/*
** EPITECH PROJECT, 2024
** commands
** File description:
** gui_cmd_smg
*/

#include <stdio.h>
#include <stdlib.h>

#include "commands.h"

void gui_cmd_smg(UNUSED server_t *server, gui_client_t *client, char *args)
{
    if (client == NULL || args == NULL)
        return;
    gui_dprintf(client, "smg %s\n", args);
}
