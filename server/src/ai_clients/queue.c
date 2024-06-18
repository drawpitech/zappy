/*
** EPITECH PROJECT, 2024
** ai_clients
** File description:
** get_client_by_id
*/

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "ai_internal.h"
#include "server.h"

static bool init_queue(ai_client_t *client)
{
    if (client->q_cmds == NULL) {
        client->q_cmds = calloc(20, sizeof *client->q_cmds);
        client->q_size = 0;
        if (client->q_cmds == NULL) {
            OOM;
            return false;
        }
    }
    if (client->q_size == 20) {
        ERR("Queue is full");
        return false;
    }
    if (client->q_size == 0)
        client->last_cmd = time(NULL);
    return true;
}

bool queue_add_cmd(ai_client_t *client, queued_cmd_t *qcmd)
{
    if (!init_queue(client))
        return false;
    client->q_cmds[client->q_size] = *qcmd;
    client->q_size += 1;
    return true;
}

bool queue_add_cmd_prio(ai_client_t *client, queued_cmd_t *qcmd)
{
    if (!init_queue(client))
        return false;
    memmove(client->q_cmds + 1, client->q_cmds, client->q_size);
    client->q_cmds[0] = *qcmd;
    client->q_size += 1;
    return true;
}

void queue_pop_cmd(server_t *server, ai_client_t *client)
{
    time_t now = time(NULL);
    queued_cmd_t *qcmd = NULL;

    if (client->q_cmds == NULL || client->q_size == 0 ||
        server->ctx.freq == 0 ||
        now - client->last_cmd < client->q_cmds[0].time / server->ctx.freq)
        return;
    client->q_size -= 1;
    qcmd = &client->q_cmds[0];
    qcmd->func(server, client, qcmd->args);
    client->last_cmd = now;
    if (qcmd->args != NULL)
        free(qcmd->args);
    memmove(client->q_cmds, client->q_cmds + 1, client->q_size);
}
