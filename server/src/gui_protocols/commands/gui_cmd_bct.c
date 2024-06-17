/*
** EPITECH PROJECT, 2024
** commands
** File description:
** gui_cmd_bct
*/

#include <stdio.h>
#include <stdlib.h>

#include "commands.h"
#include "server.h"

void gui_cmd_bct(server_t *server, gui_client_t *client, char *args)
{
    char **cmd = my_str_to_word_array(args, " ");
    vector_t cell = {0};
    size_t w = server->ctx.width;
    size_t h = server->ctx.height;

    if (cmd == NULL || !cmd[0] || !cmd[1])
        return;
    cell.x = atoi(cmd[0]);
    cell.y = atoi(cmd[1]);
    dprintf(
            client->s_fd, "bct %d %d %d %d %d %d %d %d %d\n", cell.x, cell.y,
            server->map[IDX(cell.x, cell.y, w, h)].res[FOOD].quantity,
            server->map[IDX(cell.x, cell.y, w, h)].res[LINEMATE].quantity,
            server->map[IDX(cell.x, cell.y, w, h)].res[DERAUMERE].quantity,
            server->map[IDX(cell.x, cell.y, w, h)].res[SIBUR].quantity,
            server->map[IDX(cell.x, cell.y, w, h)].res[MENDIANE].quantity,
            server->map[IDX(cell.x, cell.y, w, h)].res[PHIRAS].quantity,
            server->map[IDX(cell.x, cell.y, w, h)].res[THYSTAME].quantity
           );
    free_array((void **)cmd);
}
