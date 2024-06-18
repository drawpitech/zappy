/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** incantation
*/

#include <stdio.h>

#include "incantation.h"
#include "commands.h"
#include "../../gui_protocols/commands/commands.h"
#include "server.h"

void ai_cmd_incantation(
    server_t *server, ai_client_t *client, UNUSED char *args)
{
    int lvl = client->lvl;
    cell_t *cell = CELL(server, client->pos.x, client->pos.y);
    ai_client_t *read = NULL;
    char buffer[4096];
    char *cringe = buffer;

    for (size_t i = 0; i < LEN(INC_NEEDS[lvl].res); ++i)
        if (INC_NEEDS[lvl].res[i].quantity < cell->res[i].quantity)
            return (void)dprintf(client->s_fd, "ko\n");
    client->freezed = true;
    cringe += sprintf(buffer, "%d %d %d", cell->pos.x, cell->pos.y, lvl);
    for (size_t i = 0; i < server->ai_clients.size; ++i) {
        read = server->ai_clients.elements[i];
        if (read->pos.x == client->pos.x && read->pos.y == client->pos.y) {
            read->freezed = true;
            cringe += sprintf(cringe, " %d", read->lvl);
        }
    }
    gui_cmd_pic(server, &server->gui_client, buffer);
}
