#!/usr/bin/env python3
"""
File containing the trentorian basic commands,
those function are member of the trentorian class
"""

from trentorian.enums import (
    TrantorianDirection,
    OtherIndex,
    SoundDirection
)

from utils import (
    determine_direction,
    check_levelup,
    pack_infos,
    unpack_infos,
    split_list,
    LEVELS,
    get_incantation_team
)

from time import time
from multiprocessing import Queue

def forward(self) -> bool:
    """try to go forward, update the position if success
    time limit : 7/f

    Returns:
        bool: true if it worked, otherwise false
    """
    if self.dead:
        return False
    self.ticks += 7
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
    if self.dead:
        return False
    self.ticks += 7
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
    if self.dead:
        return False
    self.ticks += 7
    self.client.send_cmd("Left")
    answer = self.wait_answer()
    if answer != 'ok':
        return False
    self.direction = TrantorianDirection((self.direction + 1) % 4)
    return True

def look_around(self) -> bool:
    """try to look around
    time limit : 7/f

    Returns:
        bool: false if parsing failed
    """
    if self.dead:
        return False
    self.ticks += 7
    self.client.send_cmd("Look")

    cases = split_list(self.wait_answer())
    if cases == []:
        self.dprint('look parse failed')
        return False

    self.get_current_case().fill_from_str(cases.pop(0))
    direct: int = -1
    if self.direction in [TrantorianDirection.DOWN, TrantorianDirection.LEFT]:
        direct = 1
    if self.direction in [TrantorianDirection.UP, TrantorianDirection.DOWN]:
        for i in range(1, self.level + 1):
            yc = (self.y + i * direct) % self.client.size_y
            for x in range(self.x - i, self.x + 1 + i):
                self.known_map.tiles[yc][x % self.client.size_x].fill_from_str(cases.pop(0))
        return True
    for i in range(1, self.level + 1):
        xc = (self.x + i * direct) % self.client.size_x
        for y in range(self.y - i, self.y + i + 1):
            self.known_map.tiles[y % self.client.size_y][xc].fill_from_str(cases.pop(0))
    return True

def get_inventory(self) -> bool:
    """update the inventory
    time limit : 1/f

    Returns:
        bool: false if parsing failed
    """
    if self.dead:
        return False
    self.ticks += 1
    t1 = time()
    self.client.send_cmd("Inventory")
    answer = self.wait_answer()
    t2 = time()
    if self.dead:
        return False

    content = split_list(answer)
    if content == []:
        self.dprint('inventory parse faile')
        return False
    self.tick_time = ((self.tick_time * self.nbr_tests_ticks + (t2 - t1))
                    / (self.nbr_tests_ticks + 1))
    self.nbr_tests_ticks += 1
    for e in content:
        if e == '':
            continue
        key, val = e.split()
        if key not in self.inventory:
            return False
        try:
            self.inventory[key] = int(val)
        except ValueError:
            return False
    return True

def broadcast(self, content: str, receivers: list[str]) -> bool:
    """broadcast a message
    time limit : 7/f
    "sender_uuid$receiver_uuid$type$content$infos$map"

    Args:
        content (str): message content
        receivers (list[str]): list of receivers (["all"] for all, or [] for troll)

    Returns:
        bool: true if ok, otherwise false
    """
    if self.dead:
        return False
    self.ticks += 7
    self.last_broadcast = self.ticks
    self.kill_others()
    if receivers == []:
        self.client.send_cmd("Broadcast " + content)
        return self.wait_answer() == 'ok'
    msg = f'{self.team}'
    msg += f'{self.uid}$'
    msg += '|'.join(receivers) + '$'
    msg += f'{content}$'
    msg += f'{pack_infos(
        self.others, self.uid, self.inventory, self.level, self.x, self.y, self.dispo_incant)}'
    self.client.send_cmd("Broadcast " + msg)
    return self.wait_answer() == 'ok'

def get_unused_slot(self) -> bool:
    """update the unused_slot
    time limit : instant

    Returns:
        bool: false if parsing failed
    """
    if self.dead:
        return False
    self.ticks += 7
    self.client.send_cmd("Connect_nbr")
    answer = self.wait_answer()
    try:
        self.unused_slot = int(answer)
    except ValueError:
        return False
    return True

def asexual_multiplication(self, queue: Queue, place: str='no-info') -> bool:
    """selffucking for a new trantorian
    time limit : 42/f

    Args:
        queue (Queue): Queue to put in the good hole to fertilize itself
        place (str): info on where it was called
    Returns:
        bool: false if self is sterile
    """
    if self.dead:
        return False
    if self.ticks - self.last_birth < 80:
        return False
    self.last_birth = self.ticks
    self.ticks += 7
    self.client.send_cmd("Fork")
    if self.wait_answer() != 'ok':
        return False
    queue.put(f"birth {place}")
    self.dprint("birth baby")
    return True

def eject(self) -> bool:
    """eject trantorians on the same case
    time limit : 7/f

    Returns:
        bool: false if self is weak
    """
    if self.dead:
        return False
    self.ticks += 7
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
    if self.dead:
        return False
    self.ticks += 7
    self.client.send_cmd("Take " + obj)
    if self.wait_answer() != 'ok':
        return False
    self.inventory[obj] += 1
    return True

def drop_object(self, obj: str) -> bool:
    """drop object on the same case
    time limit : 7/f

    Args:
        obj (str): obj to drop

    Returns:
        bool: true if the object is took
    """
    if self.dead:
        return False
    self.ticks += 7
    self.client.send_cmd("Set " + obj)
    ans = self.wait_answer()
    if ans != 'ok':
        return False
    if obj not in self.inventory:
        return False
    self.inventory[obj] -= 1
    return True

def start_incantation(self) -> bool:
    """Start the incantation
    time limit : 300/f

    Returns:
        bool: true if the incantation did work
    """
    if self.dead:
        return False
    self.ticks += 300
    self.client.send_cmd("Incantation")
    answer = self.wait_answer()
    if answer != "Elevation underway":
        self.dprint("elev failed", answer)
        return False
    return True