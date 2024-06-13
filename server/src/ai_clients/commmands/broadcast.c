/*
** EPITECH PROJECT, 2024
** src
** File description:
** broadcast
*/

#include <stdlib.h>
#include <stdio.h>
#include "array.h"
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

static
int compute_dir(const vector_t p1, const vector_t p2, const int grid_size[2])
{
    int dx = 0;
    int dy = 0;
    sound_direction_t result = -1;

    dx = compute(p1.x, p1.x, grid_size[0]);
    dy = compute(p1.y, p1.y, grid_size[1]);
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

int broadcast(server_t *serv, const size_t idx, const char *restrict text)
{
    int dir = 0;
    ai_client_t *broadcaster = remove_elt_to_array(&serv->ai_clients, idx);
    ai_client_t *current = NULL;

    if (!broadcaster)
        return RET_ERROR;
    for (size_t i = 0; i < serv->ai_clients.nb_elements; ++i) {
        current = serv->ai_clients.elements[i];
        dir = compute_dir(broadcaster->pos, current->pos,
                (int[2]){(int)serv->ctx.width, (int)serv->ctx.height});
        dprintf(current->s_fd, "message %d, %s\n", dir, text);
    }
    return RET_VALID;
}
