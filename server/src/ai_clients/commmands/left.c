/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** ai_clients
*/

#include "commands.h"
#include "server.h"

void ai_cmd_left(
    UNUSED server_t *server, ai_client_t *client, UNUSED char *args)
{
    client->dir = MOD(client->dir - 1, 4);
    ai_write(client, "ok\n", 3);
}
