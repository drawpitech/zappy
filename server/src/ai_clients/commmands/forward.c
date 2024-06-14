/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** ai_clients
*/

#include "commands.h"
#include "server.h"

void ai_cmd_forward(server_t *server, ai_client_t *client, UNUSED char *args)
{
    CELL(server, client->pos.x, client->pos.y).res[PLAYER].quantity--;
    switch (client->dir) {
        case NORTH:
            client->pos.y = (client->pos.y - 1) % (int)server->ctx.height;
            break;
        case SOUTH:
            client->pos.y = (client->pos.y + 1) % (int)server->ctx.height;
            break;
        case EAST:
            client->pos.x = (client->pos.x + 1) % (int)server->ctx.width;
            break;
        case WEST:
            client->pos.x = (client->pos.x - 1) % (int)server->ctx.width;
            break;
    }
    CELL(server, client->pos.x, client->pos.y).res[PLAYER].quantity++;
    write(client->s_fd, "ok\n", 3);
}
