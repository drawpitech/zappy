#!/bin/python
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
