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
    char **cmd = my_str_to_word_array(args, " ");

    if (client == NULL)
        return;
    if (!cmd)
        return;
    dprintf(client->s_fd, "pic %s\n", args);
    free_array((void **)cmd);
}
