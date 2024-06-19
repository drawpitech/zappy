/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** incantation
*/

#include "incantation.h"

#include <stdio.h>
#include <time.h>

#include "../ai_internal.h"
#include "commands.h"
#include "gui_protocols/commands/commands.h"
#include "server.h"

static bool can_incantation(ai_client_t *client, const cell_t *cell)
{
    if (client->lvl >= 7 || client->lvl == 0)
        return false;
    for (size_t i = 0; i < R_COUNT; ++i)
        if (cell->res[i].quantity < INC_NEEDS[client->lvl - 1].res[i].quantity)
            return false;
    return true;
}

static void consume_ressources(
    server_t *server, ai_client_t *client, cell_t *cell, bool real_life)
{
    int qty = 0;

    if (client->lvl >= 7 || client->lvl == 0)
        return;
    for (size_t i = 0; i < R_COUNT - 2; ++i) {
        qty = INC_NEEDS[client->lvl - 1].res[i].quantity;
        cell->res[i].quantity -= qty;
        if (real_life)
            server->map_res[i].quantity -= qty;
    }
}

void ai_client_incantation_end(server_t *server, ai_client_t *client)
{
    char buffer[40];
    cell_t *cell = CELL(server, client->pos.x, client->pos.y);

    client->last_inc = 0;
    if (!can_incantation(client, cell)) {
        dprintf(client->s_fd, "ko\n");
        ERR("Uwu you lost all your money");
        return;
    }
    client->lvl += 1;
    consume_ressources(server, client, cell, true);
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
    time_t now = time(NULL);

    if (!can_incantation(client, &cell_cpy)) {
        write(client->s_fd, "ko\n", 3);
        ERR("Skill issue");
        return;
    }
    consume_ressources(server, client, &cell_cpy, false);
    client->last_inc = now;
    cringe += sprintf(buffer, "%d %d %d", cell_cpy.pos.x, cell_cpy.pos.y, lvl);
    for (size_t i = 0; i < server->ai_clients.size; ++i) {
        read = server->ai_clients.elements[i];
        if (read->pos.x == client->pos.x && read->pos.y == client->pos.y &&
            can_incantation(read, &cell_cpy)) {
            consume_ressources(server, client, &cell_cpy, false);
            client->last_inc = now;
            cringe += sprintf(cringe, " %d", read->lvl);
        }
    }
    gui_cmd_pic(server, server->gui_client, buffer);
    dprintf(client->s_fd, "Elevation underway\n");
}
