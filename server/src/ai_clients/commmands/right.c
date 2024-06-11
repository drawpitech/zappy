/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** ai_clients
*/

#include "commands.h"
#include "server.h"

void ai_cmd_right(ai_cmd_ctx_t *ctx, UNUSED char *args)
{
    ctx->ai->dir = MOD(ctx->ai->dir + 1, 4);
    write(ctx->ai->s_fd, "ok\n", 3);
}
