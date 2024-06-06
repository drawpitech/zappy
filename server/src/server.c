/*
** EPITECH PROJECT, 2024
** src
** File description:
** server
*/

#include "server.h"
#include "arg_parse.h"

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

char const * const flags[] = {
    "-p",
    "-x",
    "-y",
    "-n",
    "-c",
    "-f",
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
    client->pos.x = rand() % ctx->width;
    client->pos.y = rand() % ctx->height;
    add_elt_to_array(&server->ai_clients, client);
    dprintf(client_fd, "1\n");
    dprintf(client_fd, "%ld %ld\n", client->pos.x, client->pos.y);
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
    if (strcmp(buffer, "GRAPHIC\n") == 0) {
        // TODO implement GUI client
    } else {
        //TODO error handling
        init_ai_client(serv, ctx, client_fd, buffer);
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

static
void handle_ai_client(server_t *serv, size_t i, int client_fd)
{

    char buffer[DEFAULT_SIZE];
    ssize_t bytes_read = 0;
    ai_client_t *ai_client = NULL;

    bytes_read = read(client_fd, buffer, DEFAULT_SIZE);
    if (bytes_read <= 0) {
        remove_elt_to_array(&serv->waitlist_fd, i);
        return;
    }
    buffer[bytes_read] = '\0';
}

static
int iterate_ai_clients(server_t *server)
{
    fd_set rfd;
    struct timeval timeout;
    int client_fd = 0;

    for (size_t i = 0; i < server->ai_clients.nb_elements; ++i) {
        client_fd = ((int*)server->ai_clients.elements[i])[0];
        timeout.tv_sec = 0;
        timeout.tv_usec = 1000;
        FD_ZERO(&rfd);
        FD_SET(client_fd, &rfd);
        if (select(client_fd + 1, &rfd, NULL, NULL, &timeout) < 0)
            continue;
        if (FD_ISSET(client_fd + 1, &rfd))
            handle_ai_client(server, i, client_fd);
    }
    return 0;
}

UNUSED static void debug_ctx(context_t *ctx)
{
    printf("Port: %d\n", ctx->port);
    printf("Width: %ld\n", ctx->width);
    printf("Height: %ld\n", ctx->height);

    for (size_t i = 0; i < ctx->names->nb_elements; ++i)
        printf("%s\n", (char *)ctx->names->elements[i]);
}

static
int check_flags(const int *array, char *argv[])
{
    for (int i = 0; i < 6; ++i)
        if (strcmp(argv[array[i]], flags[i]) != 0)
            return RET_ERROR;
    return RET_VALID;
}

static
int arg_parse(const int argc, char *argv[], context_t *ctx)
{
    int adjust = (strcmp(argv[argc - 1], "-v") == 0 || strcmp(argv[argc - 1], "--verbose") == 0) ? 1 : 0;
    int array[6] = {1, 3, 5, 7, argc - 4 - adjust, argc - 2 - adjust};

    if (argc < 13)
        return RET_ERROR;
    if (check_flags(array, argv) != RET_VALID)
        return RET_ERROR;
    ctx->names = array_constructor();
    ctx->port = atoi(argv[2]);
    ctx->width = atoi(argv[4]);
    ctx->height = atoi(argv[6]);
    for (int i = 8; i < array[4]; ++i)
        add_elt_to_array(ctx->names, argv[i]);
    debug_ctx(ctx);
    return RET_VALID;
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

int server(UNUSED int argc, UNUSED char **argv)
{
    context_t ctx = {0};
    UNUSED server_t server = {0};

    if (arg_parse(argc, argv, &ctx) != RET_VALID)
        return RET_ERROR;
    if (init_server(&server, ctx.port) == RET_ERROR)
        return RET_ERROR;
    if (init_map(&server, &ctx) != RET_VALID)
        return RET_ERROR;
    /*
    for (int fd = -1;; fd = -1) {
        fd = new_client(&server);
        if (fd != -1)
            add_client(&server, fd);
        iterate_waitlist(&server);
        iterate_ai_clients(&server);
    }
    */
    // close_server(&serv);
    return RET_VALID;
}
