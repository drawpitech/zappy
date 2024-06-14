/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** ai_clients
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ai_internal.h"
#include "server.h"

void move_ai_client(server_t *server, ai_client_t *client, int dir)
{
    CELL(server, client->pos.x, client->pos.y)->res[PLAYER].quantity--;
    switch (dir) {
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
    CELL(server, client->pos.x, client->pos.y)->res[PLAYER].quantity++;
}

int init_ai_client(server_t *server, int client_fd, char *team)
{
    ai_client_t *client = malloc(sizeof(ai_client_t));

    if (client == NULL)
        return RET_ERROR;
    memset(client, 0, sizeof(ai_client_t));
    strcpy(client->team, team);
    client->s_fd = client_fd;
    client->dir = rand() % 4;
    client->pos.x = rand() % (int)server->ctx.width;
    client->pos.y = rand() % (int)server->ctx.height;
    client->lvl = 4;
    CELL(server, client->pos.x, client->pos.y)->res[PLAYER].quantity++;
    add_elt_to_array(&server->ai_clients, client);
    dprintf(client_fd, "1\n");
    dprintf(client_fd, "%d %d\n", client->pos.x, client->pos.y);
    return RET_VALID;
}

void disconnect_ai_client(ai_client_t *ai)
{
    if (!ai)
        return;
    if (ai->s_fd > 0)
        close(ai->s_fd);
    ai->s_fd = -1;
}

int remove_ai_client(server_t *server, size_t idx)
{
    ai_client_t *client = NULL;

    if (idx >= server->ai_clients.nb_elements)
        return RET_ERROR;
    client = server->ai_clients.elements[idx];
    if (client) {
        disconnect_ai_client(client);
        CELL(server, client->pos.x, client->pos.y)->res[PLAYER].quantity--;
        free(server->ai_clients.elements[idx]);
    }
    remove_elt_to_array(&server->ai_clients, idx);
    return RET_VALID;
}

int iterate_ai_clients(server_t *server)
{
    fd_set rfd;
    ai_client_t *client = NULL;

    for (size_t i = 0; i < server->ai_clients.nb_elements; ++i) {
        client = server->ai_clients.elements[i];
        if (client->s_fd < 0) {
            remove_ai_client(server, i);
            continue;
        }
        FD_ZERO(&rfd);
        FD_SET(client->s_fd, &rfd);
        if (select(
                client->s_fd + 1, &rfd, NULL, NULL,
                &(struct timeval){0, 1000}) > 0 &&
            FD_ISSET(client->s_fd, &rfd))
            handle_ai_client(server, client);
    }
    return 0;
}
