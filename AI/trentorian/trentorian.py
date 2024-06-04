#!/bin/python
"""
Module providing the trantorian class
"""
from enum import IntEnum
from warnings import warn

from multiprocessing import Queue
from client.client import Client
from trentorian.map import (
    create_default_map,
    Map,
    merge_maps
)

FOOD = 126

class TrantorianDirection(IntEnum):
    """Trantorian direction enum
    """
    UNKWOW = -1
    RIGHT = 0
    UP = 1
    LEFT = 2
    DOWN = 3

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

class Trantorian:
    """Class for a trantorian
    """
    def __init__(self, host: str, port: int, team: str):
        self.host = host
        self.port = port
        try:
            self.client = Client(host, port, team)
        except RuntimeError as err:
            print(err.args[0])
            raise RuntimeError("Trentor didn't connect to server") from err
        self.team: str = team
        self.life: int = FOOD * 10
        self.x: int = 0
        self.y: int = 0
        self.dead: bool = False
        self.inventory: dict = {
            "food": 0, "linemate": 0, "deraumere": 0,
            "sibur": 0, "mendiane": 0, "phiras": 0, "thystame": 0
        }
        self.known_map = create_default_map(10, 10)
        # TODO on the prev line, change known_map to be taken from the __init__ method
        self.direction = TrantorianDirection.RIGHT

    def born(self, queue: Queue): # TODO add a real code here
        try:
            queue.put("birth")
            while not self.dead: # all the ai code should be in this loop
                if self.forward():
                    self.x += 1
            print("died")
        except BrokenPipeError:
            print("Server closed socket")
            self.dead = True
        return

    def wait_answer(self) -> str:
        """wait for an answer, handle the message and eject

        Returns:
            str: last server answer
        """
        answer: str = self.client.get_answer()
        while answer[:7] == 'message' or answer[:5] == 'eject':
            if answer[:7] == 'message':
                self.receive_message(answer)
            if answer[:7] == 'message':
                self.handle_eject(answer)
            answer = self.client.get_answer()
        if answer[:4] == "dead":
            self.dead = True
        return answer

    def receive_message(self, msg: str):
        """handle the broadcast reception

        Args:
            msg (str): message sent by the server
        """
        print(msg)

    def handle_eject(self, msg: str):
        """handle an ejection

        Args:
            msg (str): message sent by the server
        """
        print(msg)

    def forward(self) -> bool:
        """try to go forward

        Returns:
            bool: true if it worked, otherwise false
        """
        self.client.send_cmd("Forward")
        answer = self.wait_answer()
        if answer == 'ok\n':
            return True
        return False

    def update_direction_from_msg(self, sender_x: int, sender_y: int, sound_direction: int) -> bool:
        # TODO get previous line parameter from message parsing when a protocol will be implemented
        """get the direction from the sound direction

        Args:
            sender_x (int): x of the sender
            sender_y (int): y of the sender
            sound_direction (int): direction of the sound

        Returns:
            bool: true if it could be located, otherwise false
        """

        if sender_x == 0 and sender_y == 0:
            return False

        predicted_sound_direction: int = SoundDirection.SOUTH
        # TODO calculate previous line with trigonometry

        sound_direction -= 1
        sound_direction: int = predicted_sound_direction - sound_direction

        if sound_direction < 0:
            sound_direction = sound_direction + 8

        if sound_direction % 2 != 0:
            raise ValueError("The math is not mathing: ")

        self.direction = TrantorianDirection(sound_direction / 2)

        return True

    def update_know_map_from_msg(self, new_map: Map) -> bool:
        # TODO get previous line parameter from message parsing when a protocol will be implemented
        """update the know map from a new map

        Args:
            new_map (Map): new map

        Returns:
            bool: true if it worked, otherwise false
        """
        try:
            self.known_map = merge_maps(self.known_map, new_map)
        except ValueError as err:
            warn("Warning: the map reiceived by message is not the same size as the known map: ",
            err.args[0])
            return False
        return True

if __name__ == "__main__":
    trantorien = Trantorian("localhost", 8000, "dan")
    trantorien.update_direction_from_msg(5, 5, 5)
    print(trantorien.direction)
