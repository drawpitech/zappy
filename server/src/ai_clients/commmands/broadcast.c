/*
** EPITECH PROJECT, 2024
** src
** File description:
** broadcast
*/

#include <stdio.h>
#include <stdlib.h>

#include "array.h"
#include "commands.h"
#include "../../gui_protocols/commands/commands.h"
#include "server.h"

static int compute(const int a, const int b, const int grid_size)
{
    int result = 0;
    int direct_diff = b - a;
    int wrap_diff = grid_size - abs(direct_diff);

    if (direct_diff > 0) {
        result = (direct_diff <= grid_size / 2) ? direct_diff : -wrap_diff;
        return result;
    }
    result = (abs(direct_diff) <= grid_size / 2) ? direct_diff : wrap_diff;
    return result;
}

static int compute_dir(
    const vector_t p1, const vector_t p2, const int grid_size[2])
{
    int dx = 0;
    int dy = 0;
    sound_direction_t result = -1;

    dx = compute(p1.x, p2.x, grid_size[0]);
    dy = compute(p1.y, p2.y, grid_size[1]);
    if (abs(dx) == abs(dy)) {
        if (dy < 0 && dx > 0)
            result = S_NORTH_EAST;
        if (dy < 0 && dx < 0)
            result = S_NORTH_WEST;
        if (dy > 0 && dx > 0)
            result = S_SOUTH_EAST;
        if (dy > 0 && dx < 0)
            result = S_SOUTH_WEST;
    }
    if (abs(dy) >= abs(dx)) {
        if (dy < 0)
            result = S_NORTH;
        if (dy > 0)
            result = S_SOUTH;
    }
    if (abs(dx) >= abs(dy)) {
        if (dx > 0)
            result = S_EAST;
        if (dx < 0)
            result = S_WEST;
    }
    return result;
}

void ai_cmd_broadcast(server_t *server, ai_client_t *client, char *args)
{
    unsigned int dir = 0;
    ai_client_t *current = NULL;

    for (size_t i = 0; i < server->ai_clients.nb_elements; ++i) {
        current = server->ai_clients.elements[i];
        if (current->s_fd < 0 || current == client)
            continue;
        dir = compute_dir(
            client->pos, current->pos,
            (int[2]){(int)server->ctx.width, (int)server->ctx.height});
        dir = (conv_table[client->dir] - dir) + 1;
        dprintf(current->s_fd, "message %d, %s\n", dir, args);
    }
    write(client->s_fd, "ok\n", 3);
    gui_cmd_pbc(server, server->gui_client, client->id, args);
}
