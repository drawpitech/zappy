/*
** EPITECH PROJECT, 2024
** include
** File description:
** arg_parse
*/

#pragma once

#include "server.h"

extern void (*const *FLAGS)(arg_parse_t *);

int arg_parse(int argc, char *argv[], context_t *ctx);
