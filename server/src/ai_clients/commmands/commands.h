/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** ia_internal
*/

#pragma once

#include "server.h"

void ai_cmd_forward(server_t *server, ai_client_t *client, char *args);
void ai_cmd_left(server_t *server, ai_client_t *client, char *args);
void ai_cmd_right(server_t *server, ai_client_t *client, char *args);
void ai_cmd_inventory(server_t *server, ai_client_t *client, char *args);
void ai_cmd_look(server_t *server, ai_client_t *client, char *args);
void ai_cmd_take(server_t *server, ai_client_t *client, char *args);
void ai_cmd_set(server_t *server, ai_client_t *client, char *args);
void ai_cmd_broadcast(server_t *server, ai_client_t *client, char *args);
void ai_cmd_eject(server_t *server, ai_client_t *client, char *args);
