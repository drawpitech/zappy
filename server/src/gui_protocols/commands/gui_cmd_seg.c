/*
** EPITECH PROJECT, 2024
** commands
** File description:
** gui_cmd_seg
*/

#include <stdio.h>
#include <stdlib.h>

#include "commands.h"

void gui_cmd_seg(UNUSED server_t *server, gui_client_t *client, char *args)
{
    if (!args || client == NULL)
        return;
    gui_dprintf(client, "seg %s\n", args);
}
