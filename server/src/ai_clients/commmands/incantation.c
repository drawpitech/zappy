/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** incantation
*/

#include "incantation.h"

#include <stdio.h>

#include "../ai_internal.h"
#include "commands.h"
#include "../../gui_protocols/commands/commands.h"
#include "server.h"

static bool can_incantation(ai_client_t *client, const cell_t *cell)
{
    if (client->lvl >= 7 || client->lvl == 0)
        return false;
    for (size_t i = 0; i < LEN(INC_NEEDS[client->lvl - 1].res); ++i)
        if (INC_NEEDS[client->lvl - 1].res[i].quantity < cell->res[i].quantity)
            return false;
    return true;
}

static void consume_ressources(ai_client_t *client, cell_t *cell)
{
    if (client->lvl >= 7 || client->lvl == 0)
        return;
    for (size_t i = 0; i < LEN(INC_NEEDS[client->lvl - 1].res); ++i)
        cell->res[i].quantity -= INC_NEEDS[client->lvl - 1].res[i].quantity;
}

static void incantation_end(
    server_t *server, ai_client_t *client, UNUSED char *args)
{
    char buffer[40];
    cell_t *cell = CELL(server, client->pos.x, client->pos.y);

    if (!can_incantation(client, cell)) {
        dprintf(client->s_fd, "ko\n");
        return;
    }
    consume_ressources(client, cell);
    sprintf(buffer, "%d %d", client->pos.x, client->pos.y);
    gui_cmd_bct(server, server->gui_client, buffer);
    dprintf(client->s_fd, "Current level: %d\n", client->lvl);
    gui_cmd_pie(server, server->gui_client, client->pos, 1);
}

void ai_cmd_incantation(
    server_t *server, ai_client_t *client, UNUSED char *args)
{
    int lvl = client->lvl;
    cell_t cell_cpy = *CELL(server, client->pos.x, client->pos.y);
    ai_client_t *read = NULL;
    char buffer[4096];
    char *cringe = buffer;

    if (!can_incantation(client, &cell_cpy) ||
        !queue_add_cmd(client, &(queued_cmd_t){incantation_end, 300, NULL})) {
        write(client->s_fd, "ko\n", 3);
        return;
    }
    consume_ressources(client, &cell_cpy);
    client->freezed = true;
    cringe += sprintf(buffer, "%d %d %d", cell_cpy.pos.x, cell_cpy.pos.y, lvl);
    for (size_t i = 0; i < server->ai_clients.size; ++i) {
        read = server->ai_clients.elements[i];
        if (read->pos.x == client->pos.x && read->pos.y == client->pos.y &&
            can_incantation(read, &cell_cpy) &&
            queue_add_cmd_prio(
                read, &(queued_cmd_t){incantation_end, 300, NULL})) {
            consume_ressources(client, &cell_cpy);
            read->freezed = true;
            cringe += sprintf(cringe, " %d", read->lvl);
        }
    }
    gui_cmd_pic(server, server->gui_client, buffer);
    dprintf(client->s_fd, "Elevation underway\n");
}
