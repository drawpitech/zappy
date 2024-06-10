/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** ai_clients
*/

#include "server.h"

static
void handle_ai_client(server_t *serv, size_t idx, ai_client_t *client)
{

    char buffer[DEFAULT_SIZE];
    ssize_t bytes_read = 0;

    bytes_read = read(client->s_fd, buffer, DEFAULT_SIZE);
    if (bytes_read <= 0) {
        remove_elt_to_array(&serv->waitlist_fd, idx);
        return;
    }
    buffer[bytes_read] = '\0';
}

int iterate_ai_clients(server_t *server)
{
    fd_set rfd;
    struct timeval timeout;
    ai_client_t *client = NULL;

    for (size_t i = 0; i < server->ai_clients.nb_elements; ++i) {
        client = server->ai_clients.elements[i];
        timeout.tv_sec = 0;
        timeout.tv_usec = 1000;
        FD_ZERO(&rfd);
        FD_SET(client->s_fd, &rfd);
        if (select(client->s_fd + 1, &rfd, NULL, NULL, &timeout) < 0
            || !FD_ISSET(client->s_fd + 1, &rfd))
            continue;
        handle_ai_client(server, i, client);
    }
    return 0;
}

