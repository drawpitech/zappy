/*
** EPITECH PROJECT, 2024
** src
** File description:
** server
*/

#include "server.h"

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

const double DENSITIES[] = {
    [FOOD] = 0.5,
    [LINEMATE] = 0.3,
    [DERAUMERE] = 0.15,
    [SIBUR] = 0.1,
    [MENDIANE] = 0.1,
    [PHIRAS] = 0.08,
    [THYSTAME] = 0.05
};

static int init_server(server_t *serv, int port)
{
    serv->s_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (serv->s_fd == -1)
        return RET_ERROR;
    if (setsockopt(serv->s_fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)
        ) == -1)
        return RET_ERROR;
    serv->s_addr.sin_family = AF_INET;
    serv->s_addr.sin_port = htons(port);
    if (bind(
            serv->s_fd, (struct sockaddr *)&serv->s_addr,
            sizeof serv->s_addr) == -1)
        return RET_ERROR;
    listen(serv->s_fd, 5);
    return RET_VALID;
}

static void add_client(server_t *serv, int fd)
{
    int *fd_ptr = malloc(sizeof(int));

    *fd_ptr = fd;
    add_elt_to_array(&serv->ai_clients, fd_ptr);
}

static int new_client(server_t *serv)
{
    struct timeval tv = {.tv_sec = 0, .tv_usec = 1000};
    socklen_t len = sizeof serv->s_addr;
    fd_set fdread;
    fd_set fdwrite;

    FD_ZERO(&fdread);
    FD_ZERO(&fdwrite);
    FD_SET(serv->s_fd, &fdread);
    FD_SET(serv->s_fd, &fdwrite);
    if (select(serv->s_fd + 1, &fdread, &fdwrite, NULL, &tv) <= 0)
        return -1;
    return accept(serv->s_fd, (struct sockaddr *)&serv->s_addr, &len);
}

int server(UNUSED int argc, UNUSED char **argv)
{
    context_t ctx = {0};
    server_t server = {0};

    if (init_server(&server, 6666) == RET_ERROR)
        return RET_ERROR;
    for (int fd = -1;; fd = -1) {
        fd = new_client(&server);
        if (fd != -1)
            add_client(&server, fd);
        // handle_clients(&server);
    }
    // close_server(&serv);
    return RET_VALID;
}
