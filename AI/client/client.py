#!/bin/python
"""
Module providing the connection to server and the communication functions
"""
import socket

class Client:
    """Class to handle server connection and communication
    """
    def __init__(self, ip: str, port: int, team: str):
        self.team: str = team
        if team[-1] != '\n':
            self.team += '\n'
        self.ip: str = ip
        self.port: int = port
        self.socket: socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.connect((self.ip, self.port))

        if self.get_answer() != 'WELCOME\n':
            raise RuntimeError("Server did not welcome")

        self.send_cmd(self.team)
        infos: list = self.get_answer().split('\n')
        if len(infos) != 3:
            raise RuntimeError("Server answer did't match expectation")

        x, y = infos[1].split()
        try:
            self.team_nb = int(infos[0])
            self.x = int(x)
            self.y = int(y)
        except ValueError as exc:
            raise RuntimeError("Server answer did't match expectation") from exc

    def __del__(self):
        self.socket.close()

    def get_answer(self) -> bytes:
        """get the str answer from server

        Returns:
            str: server answer
        """
        return self.socket.recv(1024).decode()

    def send_cmd(self, cmd: str) -> None:
        """send a command to the server

        Args:
            cmd (str): command to send
        """
        if cmd[-1] != '\n':
            cmd += '\n'
        self.socket.send(cmd.encode())

if __name__ == "__main__":
    ipt = "127.0.0.1"
    portt = 4242
    team_name = "TOTO"
    try:
        cli = Client(ipt, portt, team_name)
    except RuntimeError as err:
        print(err.args[0])
        exit(84)


    cli.send_cmd("Forward")
    answer = cli.get_answer()
    if answer == 'ok\n':
        print("Juste moved forward")
    else:
        print("didn't move")
