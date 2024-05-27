#!/bin/python
"""
Module providing the connection to server and the communication functions
"""
import socket

if __name__ == "__main__":
    ip = "127.0.0.1"
    port = 4242
    team_name = "TOTO" + '\n'
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client.connect((ip, port))
    donnees = client.recv(1024)
    print(donnees)
    client.send(team_name.encode())
    outs = client.recv(1024).decode().split('\n')
    if len(outs) != 2:
        print("Error while connecting to server")
    print(f"Our num is {outs[0]}")

    x, y = outs[1].split()
    try:
        x = int(x)
        y = int(y)
    except ValueError:
        print("Error while connecting to server")
    print(f"Map size x:{x}  y:{y}")


    forward = "Forward\n"
    client.send(forward.encode())
    answer = client.recv(1024)
    if answer == b'ok\n':
        print("Juste moved forward")
    else:
        print("didn't move")

    client.close()
