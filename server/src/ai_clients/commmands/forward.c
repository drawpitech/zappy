/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** ai_clients
*/

#include <stdio.h>

#include "commands.h"
#include "gui_protocols/commands/commands.h"
#include "server.h"

void ai_cmd_forward(server_t *server, ai_client_t *client, UNUSED char *args)
{
    char buf[15] = {0};

    sprintf(buf, "%d", client->id);
    gui_cmd_ppo(server, server->gui_client, buf);
    move_ai_client(server, client, client->dir);
    write(client->s_fd, "ok\n", 3);
}
