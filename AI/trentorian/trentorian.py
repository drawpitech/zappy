#!/bin/python
"""
Module providing the trantorian class
"""
from enum import IntEnum
import random
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

    def get_offset(self) -> tuple[int, int]:
        """get the offset to add to a position

        Returns:
            tuple[int, int]: x and y to change
        """
        if self == 0: # RIGHT
            return 1, 0
        if self == 1: # UP
            return 0, 1
        if self == 2: # LEFT
            return -1, 0
        if self == 3: # DOWN
            return 0, -1
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
        self.unused_slot: int = 1
        self.inventory: dict = {
            "food": 0, "linemate": 0, "deraumere": 0,
            "sibur": 0, "mendiane": 0, "phiras": 0, "thystame": 0
        }
        self.known_map = create_default_map(10, 10)
        # TODO on the prev line, change known_map to be taken from the __init__ method
        self.direction: TrantorianDirection = TrantorianDirection.RIGHT

    def born(self, queue: Queue): # TODO add a real code here
        try:
            # queue.put("birth")
            i = 0
            while not self.dead: # all the ai code should be in this loop
                if self.forward():
                    self.x += 1
                self.left()
                print(self.direction)
                # self.look_around()
                # self.take_object("food")
                # if random.randint(0, 2) == 1:
                    # self.left()
                # else : self.right()
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
            if answer[:5] == 'eject':
                self.handle_eject(answer)
            answer = self.client.get_answer()
        if answer[:4] == "dead": # TODO not sure that this is properly handled
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



##################################  COMMANDS  ##################################



    def forward(self) -> bool:
        """try to go forward, update the position if success
        time limit : 7/f

        Returns:
            bool: true if it worked, otherwise false
        """
        self.client.send_cmd("Forward")
        answer = self.wait_answer()
        if answer != 'ok':
            return False
        x, y = self.direction.get_offset()
        self.x += x
        self.y += y
        return True

    def right(self) -> bool:
        """try to turn right, update the position if success
        time limit : 7/f

        Returns:
            bool: true if it worked, otherwise false
        """
        self.client.send_cmd("Right")
        answer = self.wait_answer()
        if answer != 'ok':
            return False
        self.direction = TrantorianDirection((self.direction - 1) % 4)
        return True

    def left(self) -> bool:
        """try to turn left, update the position if success
        time limit : 7/f

        Returns:
            bool: true if it worked, otherwise false
        """
        self.client.send_cmd("Left")
        answer = self.wait_answer()
        if answer != 'ok':
            return False
        self.direction = TrantorianDirection((self.direction + 1) % 4)
        return True

    def look_around(self) -> bool: # TODO
        """try to look around
        time limit : 7/f

        Returns:
            bool: false if parsing failed
        """
        self.client.send_cmd("Look")
        answer = self.wait_answer()
        if answer[0] != '[' or answer[-1:] != "]":
            return False
        content = answer.split(",")
        print(content)
        return True

    def get_inventory(self) -> bool: # TODO
        """update the inventory
        time limit : 1/f

        Returns:
            bool: false if parsing failed
        """
        self.client.send_cmd("Inventory")
        answer = self.wait_answer()
        return True

    def broadcast(self, msg: str) -> bool: # TODO
        """broadcast a message
        time limit : 7/f

        Args:
            msg (str): message to broadcast

        Returns:
            bool: true if ok, otherwise false
        """
        self.client.send_cmd("Broadcast " + msg)
        answer = self.wait_answer()
        return answer == 'ok'

    def get_unused_slot(self) -> bool: # TODO
        """update the unused_slot
        time limit : instant

        Returns:
            bool: false if parsing failed
        """
        self.client.send_cmd("Connect_nbr")
        answer = self.wait_answer()
        try:
            self.unused_slot = int(answer)
        except ValueError:
            return False
        return True

    def asexual_multiplication(self) -> bool: # TODO
        """selffucking for a new trantorian
        time limit : 42/f

        Returns:
            bool: false if self is sterile
        """
        self.client.send_cmd("Fork")
        answer = self.wait_answer()
        return True

    def eject(self) -> bool: # TODO
        """eject trantorians on the same case
        time limit : 7/f

        Returns:
            bool: false if self is weak
        """
        self.client.send_cmd("Eject")
        answer = self.wait_answer()
        return True

    def take_object(self, obj: str) -> bool: # TODO
        """take object on the same case
        time limit : 7/f

        Args:
            obj (str): obj to take

        Returns:
            bool: true if the object is took
        """
        self.client.send_cmd("Take " + obj)
        if self.wait_answer() != 'ok':
            return False
        self.inventory[obj] += 1
        print(self.inventory[obj])
        return

    def drop_object(self, obj: str) -> bool: # TODO
        """drop object on the same case
        time limit : 7/f

        Args:
            obj (str): obj to drop

        Returns:
            bool: true if the object is took
        """
        self.client.send_cmd("Set object")
        answer = self.wait_answer()
        if answer != 'ok':
            return False
        self.inventory[obj] -= obj
        return True

    def start_incantation(self) -> bool: # TODO
        """Start the incantation
        time limit : 300/f

        Args:
            obj (str): obj to drop

        Returns:
            bool: true if the object is took
        """
        self.client.send_cmd("Incantation")
        answer = self.wait_answer()
        return True
