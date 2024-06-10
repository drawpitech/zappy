/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** ai_clients
*/

#include <stdio.h>
#include <string.h>

#include "ai_internal.h"
#include "server.h"

// TODO:
// - There will be some big issues if a users tries to send > 4096 bytes
// without a \n.

static void exec_ai_cmd(
    server_t *serv, size_t idx, ai_client_t *client, char *buffer)
{
    char *content = NULL;
    struct ai_cmd_s *cmd = NULL;

    content = strchr(buffer, ' ');
    if (content != NULL) {
        *content = '\0';
        content++;
    } else {
        content = "";
    }
    cmd = get_ai_cmd(buffer);
    if (cmd == NULL || cmd->func == NULL) {
        write(client->s_fd, "ko\n", 3);
        return;
    }
    cmd->func(serv, idx, client, content);
}

static bool process_ai_cmd(
    server_t *serv, size_t idx, ai_client_t *client, char **ptr)
{
    char *buffer = *ptr;
    char *newline = strchr(buffer, '\n');

    if (newline == NULL) {
        strcat(client->buffer, buffer);
        return false;
    }
    *newline = '\0';
    if (buffer != newline && *(newline - 1) == '\r')
        *(newline - 1) = '\0';
    exec_ai_cmd(serv, idx, client, buffer);
    *ptr = newline + 1;
    return true;
}

static void handle_ai_client(server_t *serv, size_t idx, ai_client_t *client)
{
    char buffer[sizeof client->buffer];
    ssize_t bytes_read = 0;
    size_t offset = strlen(buffer);

    strcpy(buffer, client->buffer);
    client->buffer[0] = '\0';
    bytes_read = read(client->s_fd, buffer + offset, sizeof buffer - offset);
    if (bytes_read <= 0) {
        remove_elt_to_array(&serv->ai_clients, idx);
        return;
    }
    buffer[bytes_read] = '\0';
    for (char *ptr = buffer; process_ai_cmd(serv, idx, client, &ptr);)
        ;
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
        if (select(client->s_fd + 1, &rfd, NULL, NULL, &timeout) < 0 ||
            !FD_ISSET(client->s_fd, &rfd))
            continue;
        handle_ai_client(server, i, client);
    }
    return 0;
}
