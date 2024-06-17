/*
** EPITECH PROJECT, 2024
** commands
** File description:
** gui_cmd_seg
*/

#include <stdio.h>
#include <stdlib.h>

#include "commands.h"

void gui_cmd_seg(server_t *server, gui_client_t *client, char *args)
{
    if (!args)
        return;

    dprintf(client->s_fd, "seg %s\n", args);
}
