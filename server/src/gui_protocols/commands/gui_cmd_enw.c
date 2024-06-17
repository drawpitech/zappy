/*
** EPITECH PROJECT, 2024
** commands
** File description:
** gui_cmd_enw
*/

#include <stdio.h>
#include <stdlib.h>

#include "commands.h"
#include "server.h"

void gui_cmd_enw(server_t *server, gui_client_t *client, char *args)
{
    char **cmd = my_str_to_word_array(args, " ");
    if (cmd == NULL)
        return;
    for (int i = 0; i < 4; ++i)
        if (cmd[i] == NULL)
            return;

    dprintf(
        client->s_fd, "enw #%d #%d %d %d\n", atoi(cmd[0]), atoi(cmd[1]),
        atoi(cmd[2]), atoi(cmd[3]));
    free_array((void **)cmd);
}
