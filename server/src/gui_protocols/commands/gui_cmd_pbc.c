/*
** EPITECH PROJECT, 2024
** g
** File description:
** gui_cmd_pbc
*/

#include <stdio.h>
#include <stdlib.h>

#include "commands.h"
#include "server.h"

void gui_cmd_pbc(server_t *server, gui_client_t *client, char *args)
{
    char **cmd = my_str_to_word_array(args, " ");

    if (!cmd || !cmd[0] || !cmd[1])
        return;
    dprintf(client->s_fd, "pbc #%d %s\n", atoi(cmd[0]), cmd[1]);
    free_array((void **)cmd);
}
