/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** incantation
*/

#include <stdio.h>

#include "commands.h"
#include "server.h"

void ai_cmd_incantation(
    server_t *server, ai_client_t *client, UNUSED char *args)
{
    dprintf(client->s_fd, "ko\n");
    // TODO:
    // If successful, add incantation to queue with timeout of 300
}
