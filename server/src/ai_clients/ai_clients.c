/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** ai_clients
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "ai_internal.h"
#include "bits/types/struct_timeval.h"
#include "gui_protocols/commands/commands.h"
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

int init_ai_client(server_t *serv, int client_fd, char *team, size_t egg_idx)
{
    ai_client_t *client = malloc(sizeof *client);
    egg_t *egg = serv->eggs.elements[egg_idx];

    if (client == NULL)
        return OOM, RET_ERROR;
    memset(client, 0, sizeof *client);
    strcpy(client->team, team);
    client->s_fd = client_fd;
    client->dir = rand() % 4;
    client->pos = egg->pos;
    client->lvl = 1;
    client->res[FOOD].quantity = 10;
    if (add_elt_to_array(&serv->ai_clients, client) == RET_ERROR)
        return free(client), OOM, RET_ERROR;
    client->id = serv->ai_id;
    serv->ai_id++;
    remove_elt_to_array(&serv->eggs, egg_idx);
    CELL(serv, client->pos.x, client->pos.y)->res[PLAYER].quantity++;
    CELL(serv, client->pos.x, client->pos.y)->res[EGG].quantity--;
    gui_cmd_ebo(serv, serv->gui_client, egg);
    gui_cmd_pnw(serv, serv->gui_client, client);
    dprintf(client_fd, "%zu\n", serv->ctx.client_nb - count_team(serv, team));
    dprintf(client_fd, "%ld %ld\n", serv->ctx.width, serv->ctx.height);
    return RET_VALID;
}

void disconnect_ai_client(server_t *server, ai_client_t *ai)
{
    if (!ai)
        return;
    gui_cmd_pdi(server, server->gui_client, ai->id);
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
        if (client->s_fd > 0) {
            write(client->s_fd, UNPACK("quit\n"));
            disconnect_ai_client(server, client);
        }
        CELL(server, client->pos.x, client->pos.y)->res[PLAYER].quantity--;
        free(client->buffer.str);
        free(client->q_cmds);
        free(server->ai_clients.elements[idx]);
    }
    remove_elt_to_array(&server->ai_clients, idx);
    return RET_VALID;
}

static bool starve_to_death(server_t *server, ai_client_t *ai)
{
    time_t now = time(NULL);

    if (ai->res[FOOD].quantity <= 0)
        return ERR("Starved to death"), true;
    if (ai->last_fed == 0) {
        ai->last_fed = now;
        return false;
    }
    if (server->ctx.freq >= 0 && now - ai->last_fed < 126 / server->ctx.freq) {
        ai->res[FOOD].quantity -= 1;
        ai->last_fed = now;
    }
    if (ai->res[FOOD].quantity <= 0)
        return ERR("Starved to death"), true;
    return false;
}

void iterate_ai_clients(server_t *server)
{
    fd_set rfd;
    ai_client_t *client = NULL;

    for (size_t i = 0; i < server->ai_clients.nb_elements; ++i) {
        client = server->ai_clients.elements[i];
        if (client->s_fd < 0 || starve_to_death(server, client)) {
            remove_ai_client(server, i);
            continue;
        }
        if (!client->freezed)
            queue_pop_cmd(server, client);
        FD_ZERO(&rfd);
        FD_SET(client->s_fd, &rfd);
        if (select(
                client->s_fd + 1, &rfd, NULL, NULL,
                &(struct timeval){0, 1000}) > 0 &&
            FD_ISSET(client->s_fd, &rfd))
            handle_ai_client(server, client);
    }
}
