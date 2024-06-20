/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** ai_clients
*/

#include "commands.h"
#include "../../gui_protocols/commands/commands.h"
#include "server.h"

void ai_cmd_take(server_t *server, ai_client_t *client, char *args)
{
    res_name_t res = get_ressource_type(args);
    cell_t *cell = CELL(server, client->pos.x, client->pos.y);

    if ((int)res == -1 || cell->res[res].quantity == 0) {
        ai_write(client, "ko\n", 3);
        return;
    }
    cell->res[res].quantity--;
    server->map_res[res].quantity--;
    client->res[res].quantity++;
    ai_write(client, "ok\n", 3);
    gui_cmd_pgt(server, server->gui_client, client->id, res);
    gui_cmd_mct(server, server->gui_client, NULL);
}
