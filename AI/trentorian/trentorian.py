#!/bin/python
"""
Module providing the trantorian class
"""
from time import sleep
from client.client import Client

FOOD = 126

class Trantorian:
    """Class for a trantorian
    """
    def __init__(self, host: str, port: int, team: str, pop : list):
        self.host = host
        self.port = port
        try:
            self.client = Client(host, port, team)
        except RuntimeError as err:
            print(err.args[0])
            raise RuntimeError("Trentor didn't connect to server") from err
        self.pop: list = pop
        self.pop.append(self)
        self.team: str = team
        self.life: int = FOOD * 10
        self.inventory :dict = {
            "food": 0, "linemate": 0, "deraumere": 0,
            "sibur": 0, "mendiane": 0, "phiras": 0, "thystame": 0
        }

    def born(self):
        if self.forward():
            print("Just moved forward")
        else:
            print("didn't move")
        # Trantorian(self.)

    def wait_answer(self) -> str:
        """wait for an answer, handle the message and eject

        Returns:
            str: last server answer
        """
        answer: str = self.client.get_answer()
        while answer[:7] == 'message' or answer[:5] == 'eject':
            print(answer[:7])
        return answer

    def forward(self) -> bool:
        """try to go forward

        Returns:
            bool: true if it worked, otherwise false
        """
        self.client.send_cmd("Forward")
        for i in range(100):
            print(i)
        answer = self.wait_answer()
        if answer == 'ok\n':
            return True
        return False
