#!/bin/python
"""
Module providing the trantorian class
"""
from enum import IntEnum
import random
from warnings import warn

from multiprocessing import Queue
from client.client import Client

from map import (
    create_default_map,
    Map,
    merge_maps
)

from utils import (
    determine_direction
)


############################### UTILS #########################################
def split_list(msg: str) -> list[str]:
    """convert a string of a list to the corresponding list

    Args:
        msg (str): intial string

    Returns:
        list[str]: resulting list
    """
    if msg[0] != '[' or msg[-1] != ']':
        return []
    sp = msg[1:-1].split(',')

    return [e.strip(' ') for e in sp]




#-----------------------------------------------------------------------------#
############################### THE CLASS #####################################
#-----------------------------------------------------------------------------#

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
    def __init__(self, host: str, port: int, team: str): # TODO decrement the food with time
        self.host = host
        self.port = port
        try:
            self.client = Client(host, port, team)
        except RuntimeError as err:
            print(err.args[0])
            raise RuntimeError("Trentor didn't connect to server") from err
        self.team: str = team
        self.level: int = 1
        self.x: int = 0
        self.y: int = 0
        self.dead: bool = False
        self.unused_slot: int = 1
        self.inventory: dict = {
            "food": 10, "linemate": 0, "deraumere": 0,
            "sibur": 0, "mendiane": 0, "phiras": 0, "thystame": 0
        }
        self.global_inventory: dict = {
            "food": 10, "linemate": 0, "deraumere": 0,
            "sibur": 0, "mendiane": 0, "phiras": 0, "thystame": 0
        }
        self.known_map = create_default_map(self.client.x, self.client.y)
        self.direction: TrantorianDirection = TrantorianDirection.RIGHT

    def born(self, queue: Queue): # TODO add a real code here
        try:
            # queue.put("birth")
            i = 0
            while not self.dead: # all the ai code should be in this loop
                self.get_inventory()
                # if self.forward():
                    # self.x += 1
                # self.left()
                # print(self.direction)
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


##############################  UTILS  #######################################


    def wait_answer(self) -> str: # TODO, receive "Current level" from an incantation ?"
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

    def receive_message(self, msg: str) -> None:
        """handle the broadcast reception

        Args:
            msg (str): message sent by the server
        """
        print(msg)

    def handle_eject(self, msg: str) -> None:
        """handle an ejection

        Args:
            msg (str): message sent by the server
        """
        if len(msg) != 8:
            return
        direct: TrantorianDirection = TrantorianDirection(int(msg[-1]))
        off_x, off_y = direct.get_offset()
        self.x = (self.x - off_x) % self.client.size_x # TODO check if the direction is correct
        self.y = (self.y - off_y) % self.client.size_y
        return


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

        predicted_sound_direction: int = determine_direction(
            (sender_x, sender_y), (self.x, self.y), (self.known_map.width, self.known_map.height))

        sound_direction -= 1
        sound_direction: int = predicted_sound_direction - sound_direction

        if sound_direction < 0:
            sound_direction = sound_direction + 8

        if sound_direction % 2 != 0:
            raise ValueError("The math is not mathing: ")

        self.direction = TrantorianDirection(sound_direction / 2)

        return True

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
        self.x %= self.client.size_x
        self.y %= self.client.size_y
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
        cases = split_list(self.wait_answer())
        if cases == []:
            return False
        for i in range(len(cases)): # TODO properly set the map
            print(cases[i])
        return True

    def get_inventory(self) -> bool:
        """update the inventory
        time limit : 1/f

        Returns:
            bool: false if parsing failed
        """
        self.client.send_cmd("Inventory")
        content = split_list(self.wait_answer())
        if content == []:
            return False
        newinv = {}
        for e in content:
            key, val = e.split()
            if key not in self.inventory:
                return False
            try:
                newinv[key] = int(val)
            except ValueError:
                return False
        for item, val in self.inventory:
            diff = newinv[item] - val
            self.global_inventory[item] += diff
            self.inventory = newinv[item]
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
        return self.wait_answer() == 'ok'

    def get_unused_slot(self) -> bool:
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

    def asexual_multiplication(self) -> bool:
        """selffucking for a new trantorian
        time limit : 42/f

        Returns:
            bool: false if self is sterile
        """
        self.client.send_cmd("Fork")
        if self.wait_answer() != 'ok':
            return False
        # self.broadcast("birth at my pos") # TODO send a message to other
        return True

    def eject(self) -> bool:
        """eject trantorians on the same case
        time limit : 7/f

        Returns:
            bool: false if self is weak
        """
        self.client.send_cmd("Eject")
        return self.wait_answer() == 'ok'

    def take_object(self, obj: str) -> bool:
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
        self.global_inventory[obj] += 1
        return

    def drop_object(self, obj: str) -> bool:
        """drop object on the same case
        time limit : 7/f

        Args:
            obj (str): obj to drop

        Returns:
            bool: true if the object is took
        """
        self.client.send_cmd("Set " + obj)
        if self.wait_answer() != 'ok':
            return False
        if obj not in self.inventory:
            return False
        self.inventory[obj] -= 1
        self.global_inventory[obj] -= 1
        return True

    def start_incantation(self) -> bool:
        """Start the incantation
        time limit : 300/f

        Returns:
            bool: true if the incantation did work
        """
        self.client.send_cmd("Incantation")
        if self.wait_answer() != "Elevation underway":
            return False
        answer = self.wait_answer()
        if len(answer) < 16 or answer[:15] != "Current level: " or answer[15] not in "12345678":
            return False
        self.level = int(answer[15])
        self.level += 1
        return True

if __name__ == "__main__":
    trantorien = Trantorian("localhost", 8000, "dan")
    trantorien.update_direction_from_msg(5, 5, 6)
    print(trantorien.direction)
