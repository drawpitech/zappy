/*
** EPITECH PROJECT, 2024
** commands
** File description:
** gui_cmd_pic
*/

#include <stdio.h>
#include <stdlib.h>

#include "commands.h"

void gui_cmd_pic(UNUSED server_t *server, gui_client_t *client, char *args)
{
    if (client == NULL)
        return;
    gui_dprintf(client, "pic %s\n", args);
}
