/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** ai_clients
*/

#include "commands.h"
#include "server.h"

void ai_cmd_fork(server_t *server, ai_client_t *client, UNUSED char *args)
{
    spawn_egg(server, client->team);
    write(client->s_fd, "ok\n", 3);
}
