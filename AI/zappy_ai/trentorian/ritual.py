#!/usr/bin/env python3
"""
File containing the ritual related trentorian functions,
those function are member of the trentorian class
"""

from time import time

from trentorian.commands import (
    start_incantation,
    broadcast,
    drop_object,
    right,
    left,
    forward
)
from trentorian.broadcast import (
    get_answer,
    wait_answer
)

from parser.concrete.message_type_parser import (
    MessageType,
    MessageTypeParser
)
from utils import LEVELS

def be_the_shaman(self, can_do_incant: list) -> bool:
    """call the others and do the incantation process

    Args:
        can_do_incant (list): trants to do the incantation with

    Returns:
        bool: incantation's success
    """
    self.dispo_incant = False
    beacon(self, can_do_incant)
    if self.state != "ready": # in case an other one calls him
        return False
    drop_stuff(self)
    return start_incantation(self)


def follow_the_leader(self) -> bool:
    """go and do the incantation whit the one who called

    Returns:
        bool: incantation's succes
    """
    self.dprint('Follow leader', self.last_beacon_uid)
    self.dispo_incant = False
    broadcast(self, "just$update", ["all"])
    if not follow_beacon(self):
        self.dprint("didnt go to beacon")
        return False
    drop_stuff(self)
    broadcast(self, MessageTypeParser().serialize(
        MessageType.RITUAL_READY, self), [self.last_beacon_uid])
    return True

def wait_incant(self) -> bool:
    """wait for the end of an incantation (level change or broadcast)

    Returns:
        bool: incantation failed or not
    """
    answer = ""
    while not self.dead and not answer.startswith("Current level: "): # TODO do that better
        answer = get_answer(self)
        if self.state == "wander":
            self.dprint("wander", time())#, answer)
            return False
        if answer == "incantation end":
            return False
    if self.dead or not answer.startswith("Current level: "):
        self.dprint("incant failed", answer)
        return False
    try:
        self.level = int(answer[15])
    except ValueError:
        self.dprint("incant failed", answer)
        return False
    broadcast(self, MessageTypeParser().serialize(MessageType.RITUAL_FINISH, self), "all")
    self.dprint("lvl:", self.level)
    return True
def drop_stuff(self) -> None:
    """drops all our stuff
    """
    needed = LEVELS[self.level + 1][1]
    idx: int = 0
    for obj, val in self.inventory.items():
        if obj == 'food':
            continue
        to_drop = min(val, needed[idx])
        for _ in range(to_drop):
            drop_object(self, obj)
        idx += 1
    return

def follow_beacon(self) -> bool:
    """follow the beacon to make a ritual

    Returns:
        bool: true if we are on the same case at the end
    """
    while self.iter() and self.state == "going somewhere":
        if self.last_beacon_direction == 0:
            self.state = "ritual_prep"
            self.dprint(" I am ready with", self.last_beacon_uid)
            break

        if self.last_beacon_direction < 5 and self.last_beacon_direction != 1:
            left(self)
        elif self.last_beacon_direction == 5:
            left(self)
            left(self)
        elif self.last_beacon_direction > 5:
            right(self)

        forward(self)
        self.state = "wait"
        while not self.dead and self.state == "wait":
            get_answer(self)
    return not self.dead and self.state == "ritual_prep"

def beacon(self, receivers: list) -> None:
    """set ourselves as a beacon to atract the others

    Args:
        receivers (list): people to regroup
    """
    self.state = "beacon"
    self.number_of_ritual_ready = 0
    while self.iter() and self.state == "beacon":
        self.kill_others()
        if not all(e in self.others for e in receivers):
            self.state = "wander"
            return
        broadcast(self, MessageTypeParser().serialize(MessageType.BEACON, self), receivers)
        if self.number_of_ritual_ready >= LEVELS[self.level + 1][0] - 1:
            self.dprint("we are ready", receivers)
            self.state = "ready"
    return
