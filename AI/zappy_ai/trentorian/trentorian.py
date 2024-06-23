#!/usr/bin/env python3
"""
Module providing the trantorian class
"""
from time import time
import random
from string import ascii_letters, punctuation
from multiprocessing import Queue

from zappy_ai.client import Client

from zappy_ai.parser.concrete.message_type_parser import MessageType
from zappy_ai.parser.concrete.message_type_parser import MessageTypeParser

from zappy_ai.trentorian.enums import (
    TrantorianDirection,
    OtherIndex,
)

from zappy_ai.trentorian.map import (
    create_default_map,
    MapTile,
)

from zappy_ai.utils import (
    check_levelup,
    get_incantation_team
)

################################# REST OF THE TRENTORIAN CLASS #################################

from zappy_ai.trentorian.commands import (
    forward,
    right,
    left,
    broadcast,
    start_incantation,
    look_around,
    asexual_multiplication,
    take_object,
    get_unused_slot,
    get_inventory
)

from zappy_ai.trentorian.ritual import (
    be_the_shaman,
    follow_the_leader,
    wait_incant
)

#-----------------------------------------------------------------------------#
############################### THE CLASS #####################################
#-----------------------------------------------------------------------------#

FOOD = 126
MAX_PLAYER = 12

class Trantorian:
    """Class for a trantorian
    """
    def __init__(self, host: str, port: int, team: str):
        self.host = host
        self.port = port
        try:
            self.client = Client(host, port, team)
        except (RuntimeError, ConnectionRefusedError) as err:
            raise RuntimeError("Trentor didn't connect to server") from err
        self.team: str = team
        self.level: int = 1
        self.x: int = 0
        self.y: int = 0
        self.dead: bool = False
        self.unused_slot: int = 1
        self.others: dict = {} # uid : (inv, lvl, x, y, last update, free for incant)
        self.received_messages: list = []
        self.uid: str = str(time())
        self.state: str = ""
        self.dispo_incant: bool = True # disponibility for an incantation
        self.inventory: dict = {
            "food": 10, "linemate": 0, "deraumere": 0,
            "sibur": 0, "mendiane": 0, "phiras": 0, "thystame": 0
        }
        self.known_map = create_default_map(self.client.size_x, self.client.size_y)
        self.direction: TrantorianDirection = TrantorianDirection.UP
        self.last_beacon_direction: int = 0
        self.last_beacon_uid: str = None
        self.last_beacon_time: float = 0
        self.number_of_ritual_ready: int = 0
        # least amount of food needed to go throught the longest distance and do an incantation
        self.mini_food: int = 20 + ((self.known_map.width + self.known_map.height) * 8 / FOOD)
        self.consider_dead: float = (self.known_map.width + self.known_map.height) * 8 + 400
        self.ticks: int = 0
        self.last_broadcast: int = 0
        self.last_birth: int = -80 # so it can do an incant at the spawn
        self.nbr_tests_ticks: int = 0
        self.tick_time: float = 0

    def born(self, queue: Queue):
        """launch a trantorian and do its life

        Args:
            queue (Queue): process shared queu (for birth)
        """
        try:
            self.dprint("live")
            self.start_living(queue)
            if self.dead:
                return
            self.first_level()
            while self.iter_food():
                can_level_up = self.wander(queue)
                success = False

                if self.state == "shaman":
                    self.dprint("start incant with:", can_level_up)
                    success = be_the_shaman(self, can_level_up)
                    if not success:
                        broadcast(self, MessageTypeParser().serialize(
                            MessageType.RITUAL_FAILED, self), can_level_up)

                if self.state == 'going somewhere':
                    success = follow_the_leader(self)

                if success:
                    success = wait_incant(self)
                self.dprint("incantation end", success)
                self.dispo_incant = True
                broadcast(self, "just$update", ["all"])
        except BrokenPipeError:
            self.dprint("Server closed socket")
            self.dead = True
        self.dprint("died")
        return

    def first_level(self) -> None:
        """script to fastly go to level 2 and acquire some food
        """
        ready: bool = False
        while self.iter_food() and not ready:
            look_around(self)
            if self.get_current_case().content["linemate"] > 0:
                break
            forward(self)
            direct: int = random.randint(0, 5)
            if direct == 1:
                left(self)
            elif direct == 2:
                right(self)
            ready = check_levelup(self.inventory, 2, {})
        if self.dead:
            return
        if start_incantation(self) is False:
            self.first_level()
        if wait_incant(self) is False:
            self.first_level()
        return

##############################  UTILS  #######################################


    def dprint(self, *args, **kwargs):
        """a print with the uid
        """
        print(self.uid[-6:], ':', *args, **kwargs)

    def iter(self) -> bool:
        """update the needed values and check if the next iteration is possible (not dead)

        Returns:
            bool: can do the next iteration
        """
        if self.dead:
            return False
        if self.ticks - self.last_broadcast > self.consider_dead - 20:
            broadcast(self, 'just$update', ["all"])
        if self.ticks > 10000:
            self.ticks = 0
            self.last_broadcast = 0
            self.last_birth = 0
        return True

    def iter_food(self) -> bool:
        """check if next iteration is possible, and update the needed values
        if the food is to low, refills it to 15 unit

        Returns:
            bool: can do the net iteration
        """
        if not self.iter():
            return False
        get_inventory(self)
        if self.inventory["food"] > self.mini_food:
            return True
        max_reached: bool = True
        state_change: bool = True
        under_mini: bool = True
        direct: int = 0
        while self.iter() and (under_mini or (max_reached and state_change)):
            if self.dead:
                return False
            if direct != 4 and not look_around(self):
                continue
            for _ in range(self.level): # this can be optimised
                for _ in range(self.get_current_case().content["food"]):
                    if not take_object(self, "food"):
                        break
                if self.dead:
                    return False
                forward(self)
                direct: int = random.randint(0, 8)
                if direct == 4:
                    left(self)
                    look_around(self)
            get_inventory(self)
            max_reached: bool =  self.inventory["food"] < self.mini_food + 30
            state_change: bool = self.state != "going somewhere"
            under_mini: bool = self.inventory["food"] < self.mini_food
        return not self.dead

    def get_current_case(self) -> MapTile:
        """get the case we are one

        Returns:
            MapTile: current maptile
        """
        return self.known_map.tiles[self.y % self.client.size_y][self.x % self.client.size_x]

    def wander(self, queue: Queue) -> list[str]:
        """look and take stuff until we can do an incantation, or we are called

        Args:
            queue (Queue): queue to birth other trantorians

        Returns:
            list[str]: list of players to do the incantation
        """
        self.state = "wander"
        can_level_up = []
        i = 0
        while self.iter_food() and self.state == "wander" and not can_level_up:
            get_unused_slot(self)
            if len(self.others) < MAX_PLAYER and self.unused_slot > 0:
                asexual_multiplication(self, queue, "wander")
            look_around(self)
            for _ in range(self.level): # TODO maybe some priority order here
                if not self.take_tile_objects():
                    forward(self)
                    break
                forward(self)
            direct: int = random.randint(0, 5)
            if direct == 1:
                left(self)
            elif direct == 2:
                right(self)
            can_level_up = get_incantation_team(self.inventory, self.level + 1, self.others)
            i += 1
            if not i % 4:
                continue
            self.troll_broadcasts()
            broadcast(self, "just$update", ["all"])
        if can_level_up and self.state != "going somewhere":
            self.state = "shaman"
        return can_level_up

    def start_living(self, queue: Queue) -> None:
        """ First step of the life (reproduction)

        Args:
            queue (Queue): queue to birth other trantorians
        """
        if not self.iter_food():
            return
        get_unused_slot(self)
        if self.client.team_size > 0 and len(self.others) < MAX_PLAYER and self.unused_slot > 0:
            asexual_multiplication(self, queue, "start")
        broadcast(self, "im$alive", ["all"])
        for _ in range(10): # use to set a first value for tick_time
            get_inventory(self)
        return

    def troll_broadcasts(self)-> None:
        """send weird broadcast to troll
        """
        tt = random.randint(0, 10)
        if len(self.received_messages) == 0:
            return
        if tt == 1:
            broadcast(self, '.', [])
        if tt == 2:
            length = random.randint(50, 200)
            broadcast(self, [random.choice(ascii_letters + punctuation) for _ in range(length)], ["all"])
        elif tt == 4:
            idx = random.randint(0, len(self.received_messages))
            broadcast(self, self.received_messages.pop(idx), [])
        elif tt == 3:
            idx = random.randint(0, len(self.received_messages))
            idx2 = random.randint(0, len(self.received_messages[idx]))
            broadcast(self, self.received_messages[idx][:-(idx2 / 2)], [])
        return

    def take_tile_objects(self) -> bool:
        """take all the objects on the tile

        Returns:
            bool: true if all the object where taken, false if one didn't work
        """
        content: dict = self.get_current_case().content
        if content["player"] > 1:
            return False
        for obj, quantity in content.items():
            if obj == "egg" or obj == "player":
                continue
            for _ in range(quantity):
                if take_object(self, obj):
                    return False
        return True

    def merge_others(self, received: dict) -> None:
        """merge a received list of user into the current one
        remove the ones that are to old (suspect died)

        Args:
            received (dict): received informations
        """
        dead_time: int = self.consider_dead * self.tick_time
        for uuid, infos in received.items():
            update = infos[4]
            if uuid not in self.others:
                self.others[uuid] = infos
            elif update > self.others[uuid][OtherIndex.LAST_UPDATE]:
                self.others[uuid] = infos
            diff = time() - self.others[uuid][OtherIndex.LAST_UPDATE]
            if diff > dead_time:
                self.others.pop(uuid)
        return

    def kill_others(self) -> None:
        """remove the others that are too old from the list
        """
        uids = list(self.others.keys())[::]
        for uid in uids:
            infos = self.others[uid]
            diff = time() - infos[4]
            if diff > self.consider_dead * self.tick_time:
                self.others.pop(uid)
        return

    def handle_eject(self, msg: str) -> None:
        """handle an ejection

        Args:
            msg (str): message sent by the server
        """
        if len(msg) != 8:
            return
        direct: TrantorianDirection = TrantorianDirection(int(msg[-1]))
        off_x, off_y = direct.get_offset()
        self.x = (self.x - off_x) % self.client.size_x
        self.y = (self.y - off_y) % self.client.size_y
        return

if __name__ == "__main__":
    trantorien = Trantorian("localhost", 8000, "dan")
    # trantorien.update_direction_from_msg(5, 5, 6)
    print(trantorien.direction)
