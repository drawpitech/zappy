/*
** EPITECH PROJECT, 2024
** commands
** File description:
** gui_cmd_plv
*/

#include <stdio.h>
#include <stdlib.h>

#include "commands.h"
#include "server.h"

void gui_cmd_plv(server_t *server, gui_client_t *client, char *args)
{
    char **cmd = my_str_to_word_array(args, " ");
    int client_id = atoi(*cmd);
    ai_client_t *current = get_client_by_id(server, client_id);

    if (client == NULL || current == NULL)
        return;
    dprintf(client->s_fd, "plv %d %d\n", client_id, current->lvl);
}
