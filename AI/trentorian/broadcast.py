#!/usr/bin/env python3
"""
File containing the trentorian broadcasts receptions functions,
those function are member of the trentorian class
"""

from parser.concrete.message_type_parser import (
    MessageType,
    MessageTypeParser
)

from utils import unpack_infos

def wait_answer(self) -> str:
    """wait for an answer, handle the message, eject and current level

    Returns:
        str: last server answer
    """
    answer: str = get_answer(self)
    while answer.startswith('message') or answer.startswith('eject') or answer.startswith("Current level: "):
        answer = get_answer(self)
    if answer.startswith("dead"):
        self.dead = True
    return answer

def get_answer(self) -> str:
    """wait for an answer, handle the message and eject

    Returns:
        str: last server answer
    """
    answer: str = self.client.get_answer()
    if answer.startswith('message'):
        receive_message(self, answer)
    if answer.startswith('eject'):
        self.handle_eject(answer)
    if answer.startswith("dead") or answer == '':
        self.dead = True
    return answer

def receive_message(self, msg: str) -> None:
    """handle the broadcast reception
        our messages are composed of :
        "sender_uuid$receiver_uuid$type$content$infos$map"

    Args:
        msg (str): message sent by the server
    """
    msg = msg[8:]
    direct, msg = msg.split(',')
    direct: int = int(direct)
    msg = msg.strip()
    if not msg.startswith(self.team):
        if msg.count("$") < 3:
            self.received_messages.append(msg)
        return
    msg = msg[len(self.team):]
    parts = msg.strip().split("$")
    if len(parts) != 5:
        return
    sender, receivers, msg_type, content, infos = parts
    info_unpacked = unpack_infos(infos, self.uid)
    self.merge_others(info_unpacked)
    if receivers != "all" and self.uid not in receivers.split('|'):
        return
    try:
        MessageTypeParser().deserialize(self, int(msg_type), content, direct)
    except (KeyError, ValueError):
        return
    return
