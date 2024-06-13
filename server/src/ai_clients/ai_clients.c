/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** ai_clients
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ai_internal.h"
#include "server.h"

// TODO:
// - There will be some big issues if a users tries to send > 4096 bytes
// without a \n.

static void exec_ai_cmd(server_t *server, ai_client_t *client, char *buffer)
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
    cmd->func(server, client, content);
}

static bool process_ai_cmd(server_t *server, ai_client_t *client, char **ptr)
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
    exec_ai_cmd(server, client, buffer);
    *ptr = newline + 1;
    return true;
}

static void handle_ai_client(server_t *server, ai_client_t *client)
{
    char buffer[sizeof client->buffer];
    ssize_t bytes_read = 0;
    size_t offset = strlen(buffer);

    strcpy(buffer, client->buffer);
    client->buffer[0] = '\0';
    bytes_read = read(client->s_fd, buffer + offset, sizeof buffer - offset);
    if (bytes_read <= 0) {
        disconnect_ai_client(client);
        return;
    }
    buffer[bytes_read] = '\0';
    for (char *ptr = buffer; process_ai_cmd(server, client, &ptr);)
        ;
}

int init_ai_client(server_t *server, int client_fd, char *team)
{
    ai_client_t *client = malloc(sizeof(ai_client_t));

    if (client == NULL)
        return RET_ERROR;
    memset(client, 0, sizeof(ai_client_t));
    strcpy(client->team, team);
    client->s_fd = client_fd;
    client->dir = rand() % 4;
    client->pos.x = rand() % (int)server->ctx.width;
    client->pos.y = rand() % (int)server->ctx.height;
    add_elt_to_array(&server->ai_clients, client);
    dprintf(client_fd, "1\n");
    dprintf(client_fd, "%d %d\n", client->pos.x, client->pos.y);
    return RET_VALID;
}

void disconnect_ai_client(ai_client_t *ai)
{
    if (!ai)
        return;
    if (ai->s_fd > 0)
        close(ai->s_fd);
    ai->s_fd = -1;
}

int remove_ai_client(server_t *server, size_t idx)
{
    ai_client_t *client = NULL;

    if (idx >= server->ai_clients.nb_elements)
        return RET_ERROR;
    client = server->ai_clients.elements[idx];
    if (client) {
        disconnect_ai_client(client);
        free(server->ai_clients.elements[idx]);
    }
    remove_elt_to_array(&server->ai_clients, idx);
    return RET_VALID;
}

int iterate_ai_clients(server_t *server)
{
    fd_set rfd;
    ai_client_t *client = NULL;

    for (size_t i = 0; i < server->ai_clients.nb_elements; ++i) {
        client = server->ai_clients.elements[i];
        if (client->s_fd < 0) {
            remove_ai_client(server, i);
            continue;
        }
        FD_ZERO(&rfd);
        FD_SET(client->s_fd, &rfd);
        if (select(
                client->s_fd + 1, &rfd, NULL, NULL,
                &(struct timeval){0, 1000}) > 0 &&
            FD_ISSET(client->s_fd, &rfd))
            handle_ai_client(server, client);
    }
    return 0;
}
