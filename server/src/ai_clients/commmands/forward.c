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
    move_ai_client(server, client, client->dir);
    write(client->s_fd, "ok\n", 3);
}
