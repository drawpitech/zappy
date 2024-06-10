/*
** EPITECH PROJECT, 2024
** src
** File description:
** server
*/

#include "server.h"
#include "arg_parse.h"
#include "time.h"

#include <bits/types/struct_timeval.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <string.h>
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
    listen(serv->s_fd, 10);
    return RET_VALID;
}

static void add_client(server_t *serv, int fd)
{
    int *fd_ptr = malloc(sizeof(int));

    *fd_ptr = fd;
    add_elt_to_array(&serv->ai_clients, fd_ptr);
    dprintf(fd, "WELCOME\n");
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

static
int init_ai_client(server_t *server, context_t *ctx, int client_fd, char *buffer)
{
    ai_client_t *client = malloc(sizeof(ai_client_t));

    if (client == NULL)
        return RET_ERROR;
    memset(client, 0, sizeof(ai_client_t));
    strcpy(client->team, buffer);
    client->s_fd = client_fd;
    client->dir = rand() % 4;
    client->pos.x = rand() % (int)ctx->width;
    client->pos.y = rand() % (int)ctx->height;
    add_elt_to_array(&server->ai_clients, client);
    dprintf(client_fd, "1\n");
    dprintf(client_fd, "%d %d\n", client->pos.x, client->pos.y);
    return RET_VALID;
}

static
void handle_waitlist(server_t *serv, size_t i, int client_fd, context_t *ctx)
{
    char buffer[DEFAULT_SIZE];
    ssize_t bytes_read = 0;

    bytes_read = read(client_fd, buffer, DEFAULT_SIZE);
    if (bytes_read <= 0) {
        remove_elt_to_array(&serv->waitlist_fd, i);
        return;
    }
    buffer[bytes_read] = '\0';
    if (strncmp(buffer, "GRAPHIC\n", bytes_read) == 0) {
        write(client_fd, "ko\n", 3);
        // TODO implement GUI client
    } else {
        // TODO:
        // - add client to correct team
        // - error handling
        init_ai_client(serv, ctx, client_fd, buffer);
        remove_elt_to_array(&serv->waitlist_fd, i);
    }
}

static
int iterate_waitlist(server_t *server, context_t *ctx)
{
    fd_set rfd;
    struct timeval timeout;
    int client_fd = 0;

    for (size_t i = 0; i < server->waitlist_fd.nb_elements; ++i) {
        client_fd = ((int*)server->waitlist_fd.elements[i])[0];
        timeout.tv_sec = 0;
        timeout.tv_usec = 1000;
        FD_ZERO(&rfd);
        FD_SET(client_fd, &rfd);
        if (select(client_fd + 1, &rfd, NULL, NULL, &timeout) < 0)
            continue;
        if (FD_ISSET(client_fd + 1, &rfd))
            handle_waitlist(server, i, client_fd, ctx);
    }
    return 0;
}

static void refill_map(server_t *server, context_t *ctx)
{
    size_t spread = 0;
    size_t x = 0;
    size_t y = 0;

    for (size_t i = 0; i < LEN(DENSITIES); ++i) {
        spread = (size_t)((double)ctx->map_size * DENSITIES[i]);
        for (size_t cell = 0; cell < spread; ++cell) {
            x = rand() % ctx->width;
            y = rand() % ctx->height;
            server->map[IDX(x, y, ctx->width, ctx->height)].res[i].quantity++;
        }
    }
}

static int init_map(server_t *server, context_t *ctx)
{
    ctx->map_size = ctx->width * ctx->height;
    server->map = malloc(sizeof(cell_t) * ctx->map_size);
    if (server->map == NULL)
        return RET_ERROR;
    refill_map(server, ctx);
    return RET_VALID;
}

UNUSED static void debug_payload(look_payload_t *payload)
{
    for (size_t i = 0; i < payload->size; ++i) {
        printf("%d\n", payload->cell_content[i].res[FOOD].quantity);
    }
}

int server(UNUSED int argc, UNUSED char **argv)
{
    context_t ctx = {0};
    UNUSED server_t server = {0};

    srand(time(NULL));

    if (arg_parse(argc, argv, &ctx) != RET_VALID
        || init_server(&server, ctx.port) != RET_VALID
        || init_map(&server, &ctx) != RET_VALID)
        return RET_ERROR;

    // test
    ai_client_t client = {0};
    client.lvl = 1;
    client.pos.x = 5;
    client.pos.y = 5;

    look_payload_t *payload = look(&server, &ctx, &client);
    debug_payload(payload);
    //

    for (int fd = -1;; fd = -1) {
        fd = new_client(&server);
        if (fd != -1)
            add_client(&server, fd);
        iterate_waitlist(&server, &ctx);
        iterate_ai_clients(&server);
    }
    // close_server(&serv);
    return RET_VALID;
}
