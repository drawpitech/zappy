/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** ai_clients
*/

#include "commands.h"
#include "server.h"

void ai_cmd_take(server_t *server, ai_client_t *client, char *args)
{
    res_name_t res = get_ressource_type(args);
    cell_t *cell = CELL(server, client->pos.x, client->pos.y);

    if ((int)res == -1 || cell->res[res].quantity == 0) {
        write(client->s_fd, "ko\n", 3);
        return;
    }
    cell->res[res].quantity--;
    client->res[res].quantity++;
    write(client->s_fd, "ok\n", 3);
}
