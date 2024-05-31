#!/bin/python
"""
Module providing the trantorian class
"""
from multiprocessing import Queue
from client.client import Client

FOOD = 126

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
        self.inventory :dict = {
            "food": 0, "linemate": 0, "deraumere": 0,
            "sibur": 0, "mendiane": 0, "phiras": 0, "thystame": 0
        }

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
