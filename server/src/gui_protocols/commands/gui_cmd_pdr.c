/*
** EPITECH PROJECT, 2024
** commands
** File description:
** gui_cmd_pdr
*/

#include <stdio.h>
#include <stdlib.h>

#include "commands.h"
#include "server.h"

void gui_cmd_pdr(server_t *server, gui_client_t *client, char *args)
{
    char **cmd = my_str_to_word_array(args, " \n#");
    int client_idx = 0;
    int res = 0;

    if (!cmd || !cmd[0] || !cmd[1])
        return;
    client_idx = atoi(cmd[0]);
    res = atoi(cmd[1]);
    dprintf(client->s_fd, "pdr #%d %d\n", client_idx, res);
    free_array((void **)cmd);
}
