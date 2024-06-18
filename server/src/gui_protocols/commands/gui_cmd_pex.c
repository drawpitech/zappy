/*
** EPITECH PROJECT, 2024
** commands
** File description:
** gui_cmd_pex
*/

#include <stdio.h>
#include <stdlib.h>

#include "commands.h"

void gui_cmd_pex(UNUSED server_t *server, gui_client_t *client, char *args)
{
    char **cmd = my_str_to_word_array(args, " \n#");
    int client_idx = atoi(*cmd);

    if (client == NULL)
        return;
    dprintf(client->s_fd, "pex #%d\n", client_idx);
}
