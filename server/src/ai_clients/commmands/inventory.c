/*
** EPITECH PROJECT, 2024
** commmands
** File description:
** inventory
*/

#include <stdio.h>
#include "server.h"
#include "commands.h"

static const char * const r_name[] = {
    "linemate", "deraumere", "sibur", "mendiane", "phiras", "thystame"
};

void ai_cmd_inventory(server_t *server, ai_client_t *client, UNUSED char *args)
{
    bool a = false;

    for (int i = LINEMATE; i < THYSTAME; ++i) {
        if (client->res[i].quantity == 0)
            continue;
        dprintf(1, "%s%s = %d\n", a ? ", " : "", r_name[i], client->res[i].quantity);
        a = true;
    }
}
