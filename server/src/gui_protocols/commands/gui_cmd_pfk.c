/*
** EPITECH PROJECT, 2024
** commands
** File description:
** gui_cmd_pfk
*/

#include <stdio.h>
#include <stdlib.h>

#include "commands.h"

void gui_cmd_pfk(UNUSED server_t *server, gui_client_t *client, char *args)
{
    char **cmd = my_str_to_word_array(args, " \n#");

    if (client == NULL)
        return;
    if (!cmd || !cmd[0])
        return;
    dprintf(client->s_fd, "pfk #%d\n", atoi(cmd[0]));
    free_array((void **)cmd);
}
