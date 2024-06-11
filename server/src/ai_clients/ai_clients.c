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

static void exec_ai_cmd(ai_cmd_ctx_t *ctx, char *buffer)
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
        write(ctx->ai->s_fd, "ko\n", 3);
        return;
    }
    cmd->func(ctx, content);
}

static bool process_ai_cmd(ai_cmd_ctx_t *ctx, char **ptr)
{
    char *buffer = *ptr;
    char *newline = strchr(buffer, '\n');

    if (newline == NULL) {
        strcat(ctx->ai->buffer, buffer);
        return false;
    }
    *newline = '\0';
    if (buffer != newline && *(newline - 1) == '\r')
        *(newline - 1) = '\0';
    exec_ai_cmd(ctx, buffer);
    *ptr = newline + 1;
    return true;
}

static void handle_ai_client(ai_cmd_ctx_t *ctx)
{
    char buffer[sizeof ctx->ai->buffer];
    ssize_t bytes_read = 0;
    size_t offset = strlen(buffer);

    strcpy(buffer, ctx->ai->buffer);
    ctx->ai->buffer[0] = '\0';
    bytes_read = read(ctx->ai->s_fd, buffer + offset, sizeof buffer - offset);
    if (bytes_read <= 0) {
        remove_ai_client(ctx->server, ctx->ai_idx);
        return;
    }
    buffer[bytes_read] = '\0';
    for (char *ptr = buffer; process_ai_cmd(ctx, &ptr);)
        ;
}

int init_ai_client(server_t *server, context_t *ctx, int client_fd, char *team)
{
    ai_client_t *client = malloc(sizeof(ai_client_t));

    if (client == NULL)
        return RET_ERROR;
    memset(client, 0, sizeof(ai_client_t));
    strcpy(client->team, team);
    client->s_fd = client_fd;
    client->dir = rand() % 4;
    client->pos.x = rand() % (int)ctx->width;
    client->pos.y = rand() % (int)ctx->height;
    add_elt_to_array(&server->ai_clients, client);
    dprintf(client_fd, "1\n");
    dprintf(client_fd, "%d %d\n", client->pos.x, client->pos.y);
    return RET_VALID;
}

int remove_ai_client(server_t *server, size_t idx)
{
    ai_client_t *client = NULL;

    if (idx >= server->ai_clients.nb_elements)
        return RET_ERROR;
    client = server->ai_clients.elements[idx];
    if (client) {
        if (client->s_fd > 0)
            close(client->s_fd);
        free(server->ai_clients.elements[idx]);
    }
    remove_elt_to_array(&server->ai_clients, idx);
    return RET_VALID;
}

int iterate_ai_clients(server_t *server, context_t *ctx)
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
        handle_ai_client(&(ai_cmd_ctx_t){server, ctx, i, client});
    }
    return 0;
}
