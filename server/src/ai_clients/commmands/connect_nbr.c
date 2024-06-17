/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** connect_nbr
*/

#include <stdio.h>
#include <string.h>

#include "commands.h"
#include "server.h"

void ai_cmd_connect_nbr(
    server_t *server, ai_client_t *client, UNUSED char *args)
{
    size_t count = 0;
    ai_client_t *current = NULL;

    for (size_t i = 0; i < server->ai_clients.nb_elements; ++i) {
        current = (ai_client_t *)server->ai_clients.elements[i];
        if (current->s_fd > 0 && strcmp(client->team, current->team) == 0)
            count += 1;
    }
    dprintf(client->s_fd, "%ld\n", count);
}
