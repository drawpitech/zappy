/*
** EPITECH PROJECT, 2024
** commmands
** File description:
** inventory
*/

#include <stdio.h>
#include "server.h"
#include "commands.h"

void ai_cmd_inventory(server_t *server, ai_client_t *client, UNUSED char *args)
{
    bool a = false;

    dprintf(client->s_fd, "[");
    for (int i = FOOD; i < THYSTAME; ++i) {
        if (client->res[i].quantity == 0)
            continue;
        dprintf(client->s_fd, "%s%s = %d\n", a ? ", " : "", r_name[i], client->res[i].quantity);
        a = true;
    }
    dprintf(client->s_fd, "]\n");
}
