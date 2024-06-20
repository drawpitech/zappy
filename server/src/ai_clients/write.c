/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** write
*/

#include <stdarg.h>
#include <stdio.h>

#include "ai_internal.h"
#include "server.h"

ssize_t ai_write(ai_client_t *client, const char *str, size_t n)
{
    ssize_t ret = 0;

    if (client->s_fd < 0)
        return -1;
    ret = write(client->s_fd, str, n);
    if (ret > 0)
        return ret;
    ERR("Err writing to client");
    disconnect_ai_client(client);
    return ret;
}

ssize_t ai_dprintf(ai_client_t *client, const char *fmt, ...)
{
    va_list args;
    ssize_t ret = 0;

    if (client->s_fd < 0)
        return -1;
    va_start(args, fmt);
    ret = vdprintf(client->s_fd, fmt, args);
    va_end(args);
    if (ret > 0)
        return ret;
    ERR("Err writing to client");
    disconnect_ai_client(client);
    return ret;
}
