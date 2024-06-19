/*
** EPITECH PROJECT, 2024
** src
** File description:
** free_array
*/

#include <stdlib.h>

void free_array(void **array)
{
    for (size_t i = 0; array[i] != NULL; ++i) {
        free(array[i]);
    }
    free(array);
}
