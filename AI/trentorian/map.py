#!/bin/python
"""
Module providing the Map class
"""

import dataclasses
from time import time

NEVER_UPDATED = 0

def default_map_tile_content() -> dict[str: int]:
    """dictionnary representing an empty tile

    Returns:
        dict: empty dict
    """
    return {
        "food": 0, "egg": 0,
        "linemate": 0, "deraumere": 0, "sibur": 0,
        "mendiane": 0, "phiras": 0, "thystame": 0,
        "player": 0
    }

@dataclasses.dataclass
class MapTile:
    """Class for a map tile
    """
    x: int
    y: int
    last_update: float
    content: dict

    def __iter__(self):
        return iter((self.x, self.y, self.last_update, self.content))

    def __str__(self) -> str:
        total: str = f'{self.x}-{self.y}-{self.last_update}-'
        total += '-'.join(map(str, self.content.values()))
        return total

    def fill_from_str(self, content_str: str) -> None:
        """Update a tile from its new content

        Args:
            content_str (str): string of the content, as it is sended by the server
        """
        objects = content_str.strip().split(' ')
        self.content = default_map_tile_content()
        for obj in objects:
            if obj not in self.content.keys():
                continue
            self.content[obj] += 1
        self.last_update = time()
        return


    @classmethod
    def from_str(cls, map_tile_str: str) -> "MapTile":
        """Creates a MapTile object from its string representation.

        Args:
            map_tile_str (str): string representation of a MapTile

        Raises:
            SyntaxError: for error in parsing

        Returns:
            MapTile: tile unoacked from the str
        """
        parts = map_tile_str.strip().split('-')
        if len(parts) != 12:
            raise SyntaxError("Invalid MapTile string format")

        try:
            x = int(parts[0])
            y = int(parts[1])
            last_update = float(parts[2])
        except ValueError as err:
            raise SyntaxError("Invalid numerical values in MapTile string") from err

        content: dict[str: int] = default_map_tile_content()
        try:
            content["food"] = int(parts[3])
            content["egg"] = int(parts[4])
            content["linemate"] = int(parts[5])
            content["deraumere"] = int(parts[6])
            content["sibur"] = int(parts[7])
            content["mendiane"] = int(parts[8])
            content["phiras"] = int(parts[9])
            content["thystame"] = int(parts[10])
            content["player"] = int(parts[11])
        except (SyntaxError, ValueError) as err:
            raise SyntaxError("Invalid content dictionary format in MapTile string") from err

        return cls(x, y, last_update, content)

@dataclasses.dataclass
class Map:
    """Class for a map
    """
    width: int
    height: int
    tiles: list[list[MapTile]]

    def __str__(self) -> str:
        total: str = f'{self.width}|{self.height}|'
        total += '|'.join(str(MapTile(*t)) for line in self.tiles for t in line )
        return total

    @classmethod
    def from_str(cls, map_str: str) -> "Map":
        """Creates a Map object from its string representation.

        Args:
            map_str (str): String representation of the map

        Raises:
            SyntaxError: Error raised if parsing failed

        Returns:
            Map: resulting map
        """
        parts: list[str] = map_str.strip().split('|')
        if len(parts) < 3:
            raise SyntaxError('Invalid string')
        try:
            width = int(parts[0])
            height = int(parts[1])
        except ValueError as err:
            raise SyntaxError("Invalid map size") from err
        parts = parts[2:]
        if len(parts) < width * height:
            raise SyntaxError("Invalid map size") from err

        tiles: list[list[MapTile]] = []
        try:
            for i in range(height):
                tiles.append([])
                for j in range(width):
                    tiles[-1].append(MapTile.from_str(parts[i * width + j]))
        except (SyntaxError, ValueError) as err:
            raise SyntaxError("Invalid map tiles") from err
        return Map(width, height, tiles)

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
            first_map.tiles[y][x] = merge_map_tiles(first_map.tiles[y][x], second_map.tiles[y][x])

    return first_map

def create_default_map(width: int, height: int) -> Map:
    """Create a default map

    Args:
        width (int): width of the map
        height (int): height of the map

    Returns:
        Map: default map
    """
    tiles_map: list[MapTile] = []
    for y in range(height):
        tiles_map.append([])
        for x in range(width):
            tiles_map[y].append(MapTile(x, y, NEVER_UPDATED, default_map_tile_content()))
    default_map : Map = Map(width, height, tiles_map)
    return default_map




if __name__ == "__main__":
    # tile = MapTile(1, 2, NEVER_UPDATED, default_map_tile_content(), [])
    # tilestr = str(tile)
    # print(tilestr)
    # tt = MapTile.from_str(tilestr)
    # print(tt)
    mapp: Map = create_default_map(6, 6)
    mappstr = str(mapp)
    print(mappstr)
    print()
    mm = Map.from_str(mappstr)
    print(mm)
