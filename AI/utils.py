#!/usr/bin/env python3
"""
Module providing some utilities for the ai
"""

from enum import IntEnum

class SoundDirection(IntEnum):
    """Sound direction enum
    """
    EAST = 0
    NORTH_EAST = 1
    NORTH = 2
    NORTH_WEST = 3
    WEST = 4
    SOUTH_WEST = 5
    SOUTH = 6
    SOUTH_EAST = 7

def shortest_path_diff(a: int, b: int, size: int) -> int:
    """get the shortest path difference

    Args:
        a (int): first value
        b (int): second value
        size (int): size of the grid

    Returns:
        int: shortest path difference
    """
    direct_diff: int = b - a
    wrap_diff: int = size - abs(direct_diff)

    if direct_diff > 0:
        return direct_diff if direct_diff <= size // 2 else -wrap_diff

    return direct_diff if abs(direct_diff) <= size // 2 else wrap_diff

def determine_direction(p1, p2, grid_size) -> SoundDirection:
    # TODO: fix this shit
    """g
    """
    dx: int = shortest_path_diff(p1[0], p2[0], grid_size[0])
    dy: int = shortest_path_diff(p1[1], p2[1], grid_size[1])

    direction = -1

    if abs(dx) == abs(dy):
        if dy < 0 and dx > 0:
            direction = SoundDirection.NORTH_EAST
        elif dy < 0 and dx < 0:
            direction = SoundDirection.NORTH_WEST
        elif dy > 0 and dx > 0:
            direction = SoundDirection.SOUTH_EAST
        elif dy > 0 and dx < 0:
            direction = SoundDirection.SOUTH_EAST

    elif abs(dy) >= abs(dx):
        if dy < 0:
            direction = SoundDirection.NORTH
        elif dy > 0:
            direction = SoundDirection.SOUTH

    elif abs(dx) >= abs(dy):
        if dx > 0:
            direction = SoundDirection.EAST
        elif dx < 0:
            direction = SoundDirection.WEST

    return direction

def check_levelup(inventory: dict, level: int, current_players: int) -> bool:
    """check if the level up is possible given the current ressources

    Args:
        inventory (dict): current ressources
        level (int): objective level
        current_players (int): number of player on the same level

    Returns:
        bool: true if incantation is possible
    """
    elements = ["food", "linemate", "deraumere", "sibur", "mendiane", "phiras", "thystame"]
    for elt in elements:
        if elt not in inventory:
            return False
    levels = { # contain for each level, nbr player needed and ressource needed
        2: (1,[1, 0, 0, 0, 0, 0]),
        3: (1,[1, 1, 1, 0, 0, 0]),
        4: (1,[2, 0, 1, 0, 2, 0]),
        5: (1,[1, 1, 2, 0, 1, 0]),
        6: (1,[1, 2, 1, 3, 0, 0]),
        7: (1,[1, 2, 3, 0, 1, 0]),
        8: (1,[2, 2, 2, 2, 2, 1]),
    }
    nbplayers, materials = levels[level]
    if nbplayers > current_players:
        return False
    if inventory["food"] < 3 * nbplayers:
        return False
    for i in range(len(elements) - 1):
        if inventory[elements[i + 1]] < materials[i]:
            return False
    return True
