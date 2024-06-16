/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** ai_clients
*/

#include <stdlib.h>

#include "commands.h"
#include "server.h"

void ai_cmd_fork(server_t *server, ai_client_t *client, UNUSED char *args)
{
    egg_t *egg = malloc(sizeof *egg);

    if (!egg) {
        OOM;
        return;
    }
    egg->pos = (vector_t){
        .x = rand() % (int)server->ctx.width,
        .y = rand() % (int)server->ctx.height,
    };
    egg->team = client->team;
    if (add_elt_to_array(&server->eggs, egg) == RET_ERROR) {
        free(egg), OOM;
        return;
    }
    CELL(server, egg->pos.x, egg->pos.y)->res[EGG].quantity += 1;
    write(client->s_fd, "ok\n", 3);
}
