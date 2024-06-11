/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** ai_clients
*/

#include "commands.h"
#include "server.h"

void ai_cmd_forward(ai_cmd_ctx_t *ctx, UNUSED char *args)
{
    switch (ctx->ai->dir) {
        case NORTH:
            ctx->ai->pos.y = (ctx->ai->pos.y - 1) % (int)ctx->ctx->height;
            break;
        case EAST:
            ctx->ai->pos.x = (ctx->ai->pos.x + 1) % (int)ctx->ctx->width;
            break;
        case SOUTH:
            ctx->ai->pos.y = (ctx->ai->pos.y + 1) % (int)ctx->ctx->height;
            break;
        case WEST:
            ctx->ai->pos.x = (ctx->ai->pos.x - 1) % (int)ctx->ctx->width;
            break;
    }
    write(ctx->ai->s_fd, "ok\n", 3);
}
