#!/usr/bin/env python3
"""
Module providing the Trentorian utils enums
"""

from enum import IntEnum

class TrantorianDirection(IntEnum):
    """Trantorian direction enum
    """
    UNKWOW = -1
    RIGHT = 0
    UP = 1
    LEFT = 2
    DOWN = 3

    def get_offset(self) -> tuple[int, int]:
        """get the offset to add to a position

        Returns:
            tuple[int, int]: x and y to change
        """
        if self == 0: # RIGHT
            return 1, 0
        if self == 1: # UP
            return 0, -1
        if self == 2: # LEFT
            return -1, 0
        if self == 3: # DOWN
            return 0, 1
        return 0, 0

    def __str__(self) -> str:
        if self == 0:
            return "RIGHT"
        if self == 1:
            return "UP"
        if self == 2:
            return "LEFT"
        if self == 3:
            return "DOWN"
        return "UNKWOW"

class OtherIndex(IntEnum):
    """Other index enum
    """
    INV = 0
    LVL = 1
    X = 2
    Y = 3
    LAST_UPDATE = 4

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
