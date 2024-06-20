/*
** EPITECH PROJECT, 2024
** commmands
** File description:
** inventory
*/

#include "commands.h"
#include "server.h"
#include "commands.h"

void ai_cmd_inventory(server_t *server, ai_client_t *client, UNUSED char *args)
{
    bool a = false;

    ai_dprintf(client, "[");
    for (int i = FOOD; i < THYSTAME; ++i) {
        if (client->res[i].quantity == 0)
            continue;
        ai_dprintf(
            client, "%s%s %d", a ? ", " : "", r_name[i],
            client->res[i].quantity);
        a = true;
    }
    ai_dprintf(client, "]\n");
}
