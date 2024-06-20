/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** connect_nbr
*/

#include <stdio.h>

#include "commands.h"
#include "server.h"

void ai_cmd_connect_nbr(
    server_t *server, ai_client_t *client, UNUSED char *args)
{
    dprintf(
        client->s_fd, "%zu\n",
        server->ctx.client_nb - count_team(server, client->team));
}
