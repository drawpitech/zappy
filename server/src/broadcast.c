/*
** EPITECH PROJECT, 2024
** src
** File description:
** broadcast
*/

#include "server.h"

int broadcast(server_t *serv, context_t *ctx, size_t id, const char *text)
{
    for (size_t i = 0; i < serv->ai_clients.nb_elements; ++i) {
        if (i == id)
            continue;
    }
    return RET_VALID;
}
