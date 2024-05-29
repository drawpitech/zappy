#!/bin/python
"""
Module providing the trantorian class
"""
from client.client import Client

class Trantorian:
    """Class for a trantorian
    """
    def __init__(self, host: str, port: int, team: str, pop : list):
        try:
            self.client = Client(host, port, team)
        except RuntimeError as err:
            print(err.args[0])
            raise RuntimeError("Trentor didn't connect to server") from err
        self.pop: list = pop
        self.pop.append(self)
        self.team: str = team
        self.life: int = 10
        self.inventory :dict = {
            "food": 0, "linemate": 0, "deraumere": 0,
            "sibur": 0, "mendiane": 0, "phiras": 0, "thystame": 0
        }

    def forward(self) -> bool:
        """try to go forward

        Returns:
            bool: true if it worked, otherwise false
        """
        self.client.send_cmd("Forward")
        answer = self.client.get_answer()
        if answer == 'ok\n':
            return True
        return False
