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
    int id = 0;
    ai_client_t *current = NULL;

    if (client == NULL || current == NULL)
        return;
    id = atoi(*cmd);
    current = get_client_by_id(server, id);
    if (current == NULL)
        return;
    gui_dprintf(client, "plv %d %d\n", id, current->lvl);
}
