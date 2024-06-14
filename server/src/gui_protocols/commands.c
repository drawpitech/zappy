/*
** EPITECH PROJECT, 2024
** gui_protocols
** File description:
** commands
*/

#include "commands/commands.h"
#include "gui_internal.h"

static const struct gui_cmd_s commands[] = {
    {"msz", gui_cmd_msz}
};


const struct gui_cmd_s *const GUI_CLIENT_COMMANDS = commands;
const size_t GUI_CLIENT_COMMANDS_LEN = LEN(commands);
