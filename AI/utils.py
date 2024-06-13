#!/bin/python
"""
Module providing some utilities for the ai
"""

from enum import IntEnum
from time import time

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
            direction = SoundDirection.SOUTH_WEST

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


def pack_infos(players: dict, uid: float, inventory: dict, lvl: int, x: int, y: int) -> str:
    """convert the playrs infos to a string

    Args:
        players (dict): dictionnary of all the teams members
        uid (float): player uid
        inventory (dict): player inventory
        lvl (int): player level
        x (int): player pos x
        y (int): player pos y

    Returns:
        str: packed infos
    """
    total: str = ""
    for puid, infos in players:
        inv, plvl, px, py, update = infos

        total += f"{puid} {' '.join(map(str, inv.values()))} {plvl} {px} {py} {update}|"
    total += f"{uid} {' '.join(map(str, inventory.values()))} {lvl} {x} {y} {time()}"

def unpack_inventory(msg: str) -> dict:
    """parse a string to an inventory

    Args:
        msg (str): string to parse

    Returns:
        dict: obateined inventory
    """
    inv = msg.strip().split()
    res: dict = {
            "food": 0, "linemate": 0, "deraumere": 0,
            "sibur": 0, "mendiane": 0, "phiras": 0, "thystame": 0
    }
    try:
        res["food"] = int(inv[0])
        res["linemate"] = int(inv[1])
        res["deraumere"] = int(inv[2])
        res["sibur"] = int(inv[3])
        res["mendiane"] = int(inv[4])
        res["phiras"] = int(inv[5])
        res["thystame"] = int(inv[6])
    except (SyntaxError, ValueError, IndexError) as err:
        raise SyntaxError("Invalid dictionary format") from err
    return res

def unpack_infos(msg: str, uid: str) -> dict:
    """convert a string to a dic of the others players infos

    Args:
        msg (str): string to parse
        uid (str): player uuid to skip

    Returns:
        dict: result
    """
    content: list = msg.strip().split('|')
    res: dict = {}
    for players in content:
        infos = players.strip().split(' ')
        if len(infos) != 5:
            continue              # TODO raise an error
        uuid, inv, lvl, x, y, last_u = infos
        if uuid == uid:
            continue
        try:
            inventory =  unpack_inventory(inv)
            lvl = int(lvl)
            x = int(x)
            y = int(y)
            last_u = float(last_u)
        except (SyntaxError, ValueError):
            continue
        res[uuid] = (inventory, lvl, x, y, last_u)
    return res


def split_list(msg: str) -> list[str]:
    """convert a string of a list to the corresponding list

    Args:
        msg (str): intial string

    Returns:
        list[str]: resulting list
    """
    if len(msg) < 2 or msg[0] != '[' or msg[-1] != ']':
        print("Parsing failed")
        return []
    sp = msg[1:-1].split(',')

    return [e.strip(' ') for e in sp]
