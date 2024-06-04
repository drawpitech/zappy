#!/bin/python
"""
Module providing the Map class
"""

import dataclasses

NEVER_UPDATED = -1

@dataclasses.dataclass
class PlayerMapInfo:
    """Class for a player map info
    """
    direction: int
    team: int

@dataclasses.dataclass
class MapTile:
    """Class for a map tile
    """
    x: int
    y: int
    last_update: int
    content: dict
    player_infos: list[PlayerMapInfo]

@dataclasses.dataclass
class Map:
    """Class for a map
    """
    width: int
    height: int
    tiles: list[MapTile]

def merge_map_tiles(first_tile: MapTile, second_tile: MapTile) -> MapTile:
    """Merge two map tiles

    Args:
        first_tile (MapTile): first tile to merge
        second_tile (MapTile): second tile to merge
    
    Returns:
        MapTile: merged tile
    """
    if first_tile.last_update > second_tile.last_update:
        return first_tile
    if first_tile.last_update == NEVER_UPDATED:
        return first_tile

    return second_tile

def merge_maps(first_map: Map, second_map: Map) -> Map:
    """Merge two maps

    Args:
        first_map (Map): first map to merge
        second_map (Map): second map to merge
    
    Returns:
        Map: merged map
    """
    if first_map.width != second_map.width or first_map.height != second_map.height:
        raise ValueError("Maps must have the same dimensions to be merged")

    for y in range(first_map.height):
        for x in range(first_map.width):
            first_map.tiles[y][x] = merge_map_tiles(first_map.tiles[y][x], second_map[y][x])

    return first_map

def create_default_map(width: int, height: int) -> Map:
    """Create a default map

    Args:
        width (int): width of the map
        height (int): height of the map

    Returns:
        Map: default map
    """
    default_map_tile_content: dict = {
        "food": 0,
        "linemate": 0, "deraumere": 0, "sibur": 0, 
        "mendiane": 0, "phiras": 0, "thystame": 0,
        "player": 0
    }
    tiles_map: list[MapTile] = []
    for y in range(height):
        tiles_map.append([])
        for x in range(width):
            tiles_map[y].append(MapTile(x, y, NEVER_UPDATED, default_map_tile_content, []))
    default_map : Map = Map(width, height, tiles_map)
    return default_map
