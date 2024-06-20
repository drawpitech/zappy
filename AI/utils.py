#!/usr/bin/env python3
"""
Module providing some utilities for the ai
"""

from enum import IntEnum
from time import time
from itertools import combinations

LEVELS = { # contain for each level, nbr player needed and ressource needed
    2: (1,[1, 0, 0, 0, 0, 0]),
    3: (2,[1, 1, 1, 0, 0, 0]),
    4: (2,[2, 0, 1, 0, 2, 0]),
    5: (4,[1, 1, 2, 0, 1, 0]),
    6: (4,[1, 2, 1, 3, 0, 0]),
    7: (6,[1, 2, 3, 0, 1, 0]),
    8: (6,[2, 2, 2, 2, 2, 1]),
}

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

def check_levelup(inventory: dict, level: int, current_players: dict) -> bool:
    """check if the level up is possible given the current ressources

    Args:
        inventory (dict): player inventory
        level (int): objective level
        current_players (dict): team

    Returns:
        bool: true if incantation is possible
    """
    if level > 8:
        return False
    nbplayers, materials = LEVELS[level]
    global_inv = {"food":0, "linemate":0, "deraumere":0,
                  "sibur":0, "mendiane":0, "phiras":0, "thystame":0}
    if nbplayers > len(current_players) + 1:
        return False
    for key in global_inv:
        global_inv[key] += inventory[key]
        global_inv[key] += sum(p[0][key] for p in current_players.values())
    if global_inv["food"] < 3 * nbplayers:
        return False

    global_val = list(global_inv.values()) # 1: to eject the food
    global_val.pop(0) # remove the food
    if len(global_val) != len(materials):
        return False
    return all(a >= b for a, b in zip(global_val, materials))


def pack_infos(players: dict, uid: float, inventory: dict, lvl: int, x: int, y: int, incant: bool) -> str:
    """convert the playrs infos to a string

    Args:
        players (dict): dictionnary of all the teams members
        uid (float): player uid
        inventory (dict): player inventory
        lvl (int): player level
        x (int): player pos x
        y (int): player pos y
        incant (bool): disponibility for incantation

    Returns:
        str: packed infos
    """
    total: str = ""
    for puid, infos in players.items():
        inv, plvl, px, py, update, pincant = infos

        total += f"{puid}-{'%'.join(map(str, inv.values()))}-{plvl}-{px}-{py}-{update}-{pincant}|"
    total += f"{uid}-{'%'.join(map(str, inventory.values()))}-{lvl}-{x}-{y}-{time()}-{incant}"
    return total

def unpack_inventory(msg: str) -> dict:
    """parse a string to an inventory

    Args:
        msg (str): string to parse

    Returns:
        dict: obateined inventory
    """
    inv = msg.strip().split('%')
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
        infos = players.strip().split('-')
        if len(infos) != 7:
            continue              # TODO raise an error
        uuid, inv, lvl, x, y, last_u, incant = infos
        if uuid == uid:
            continue
        try:
            inventory =  unpack_inventory(inv)
            lvl = int(lvl)
            x = int(x)
            y = int(y)
            last_u = float(last_u)
            incant = incant == 'True'
        except (SyntaxError, ValueError):
            continue
        res[uuid] = (inventory, lvl, x, y, last_u, incant)
    return res


def split_list(msg: str) -> list[str]:
    """convert a string of a list to the corresponding list

    Args:
        msg (str): intial string

    Returns:
        list[str]: resulting list
    """
    if len(msg) < 2 or msg[0] != '[' or msg[-1] != ']':
        print(msg)
        print("List parsing failed")
        return []
    sp = msg[1:-1].split(',')

    return [e.strip(' ') for e in sp]

def get_all_subsets(data: list, n: int) -> list[list]:
    """
    This function iteratively generates all subsets of size n from the given list.

    Args:
        data: The list from which to generate subsets.
        n: The desired size of the subsets.

    Returns:
        A list containing all subsets of size n from the input list.
    """
    subsets = []
    for i in range(len(data) ** n):  # Iterate through all possible combinations of indices
        subset = []
        # Convert index to base-n representation (digits represent chosen elements)
        index_str = bin(i)[2:].zfill(n)  # Pad with zeros to ensure n digits
        for j in range(n):
            if index_str[j] == '1':
                subset.append(data[j])
    subsets.append(subset)
    return subsets

def get_incantation_team(inventory: dict, level: int, current_players: int) -> list:
    """get the list of players for the incantation

    Args:
        inventory (dict): player inventory
        level (int): objective level
        current_players (int): players of the team

    Returns:
        list: uid of the players needed for the incantation, empty if no one
    """
    if level > 8:
        return []
    same_levels: dict = dict(
        filter(lambda item: item[1][1] == level - 1 and item[1][5], current_players.items()))
    possibilities = combinations(same_levels.items(), LEVELS[level][0] - 1)

    for possible in possibilities:
        possible = dict(possible)
        if check_levelup(inventory, level, possible):
            return list(possible.keys())
    return []
