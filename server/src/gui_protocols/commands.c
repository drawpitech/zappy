/*
** EPITECH PROJECT, 2024
** gui_protocols
** File description:
** commands
*/

#include "commands/commands.h"
#include "gui_internal.h"

static const struct gui_cmd_s commands[] = {
    {"msz", gui_cmd_msz},
    {"bct", gui_cmd_bct},
    {"mct", gui_cmd_mct},
    {"tna", gui_cmd_tna},
    {"ppo", gui_cmd_ppo},
    {"plv", gui_cmd_plv},
    {"pin", gui_cmd_pin},
    {"sgt", gui_cmd_sgt},
    {"sst", gui_cmd_sst},
};


const struct gui_cmd_s *const GUI_CLIENT_COMMANDS = commands;
const size_t GUI_CLIENT_COMMANDS_LEN = LEN(commands);
