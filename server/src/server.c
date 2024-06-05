/*
** EPITECH PROJECT, 2024
** src
** File description:
** server
*/

#include "server.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include "array.h"

const double DENSITIES[] = {
    [FOOD] = 0.5,
    [LINEMATE] = 0.3,
    [DERAUMERE] = 0.15,
    [SIBUR] = 0.1,
    [MENDIANE] = 0.1,
    [PHIRAS] = 0.08,
    [THYSTAME] = 0.05
};

static int init_server(server_t *server, int port)
{
    struct sockaddr_in a = {0};

    server->s_fd = socket(AF_INET, SOCK_STREAM, 0);
    server->ai_clients = array_constructor();
    if (server->s_fd == -1)
        return RET_ERROR;
    a.sin_family = AF_INET;
    a.sin_port = htonl(port);
    if (bind(server->s_fd, (struct sockaddr *)&a, sizeof(a)) == -1)
        return RET_ERROR;
    listen(server->s_fd, 5);
    return RET_VALID;
}

int server(UNUSED int argc, UNUSED char **argv)
{
    context_t ctx = {0};
    server_t server = {0};

    if (init_server(&server, 6666) == RET_ERROR)
        return RET_ERROR;
    return RET_VALID;
}
