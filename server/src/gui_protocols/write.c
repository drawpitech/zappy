/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** write
*/

#include <stdarg.h>
#include <stdio.h>

#include "server.h"

ssize_t gui_write(gui_client_t *gui, const char *str, size_t n)
{
    ssize_t ret = 0;

    if (gui->s_fd < 0)
        return -1;
    ret = write(gui->s_fd, str, n);
    if (ret > 0)
        return ret;
    ERR("Err writing to gui");
    close(gui->s_fd);
    gui->s_fd = -1;
    return ret;
}

ssize_t gui_dprintf(gui_client_t *gui, const char *fmt, ...)
{
    va_list args;
    ssize_t ret = 0;

    if (gui->s_fd < 0)
        return -1;
    va_start(args, fmt);
    ret = vdprintf(gui->s_fd, fmt, args);
    va_end(args);
    if (ret > 0)
        return ret;
    ERR("Err writing to gui");
    close(gui->s_fd);
    gui->s_fd = -1;
    return ret;
}
