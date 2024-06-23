#!/usr/bin/env python3
""" Main """
from multiprocessing import Process, Queue
import typer
from typing_extensions import Annotated

from zappy_ai.args_handling import get_args
from zappy_ai.trentorian import Trantorian


def burry_dead(pop: list[Process]) -> list[Process]:
    """remove all the dead trantorians from the population

    Args:
        pop (list[Process]): list of trantorians

    Returns:
        list[Process]: filtered list
    """
    return list(filter(lambda trant:trant.is_alive(), pop))


def launch_ai(host: str, port: int, team: str):
    """start the first ai

    Args:
        host (str): machine hosting serv
        port (int): port open for the serv
        team (str): name of the team
    """
    threads: list[Process] = []
    queue: Queue = Queue()
    t = Trantorian(host, port, team)
    threads.append(Process(target=t.born, args=(queue,)))

    for e in threads:
        e.start()

    while len(threads) > 0:
        threads = burry_dead(threads)
        if queue.empty():
            continue
        sended = queue.get()
        if sended.startswith("birth"):
            try:
                t = Trantorian(host, port, team)
            except RuntimeError:
                print("Trantor was aborted")
            threads.append(Process(target=t.born, args=(queue,)))
            threads[-1].start()
    return

def main_typer(port: int = typer.Option(..., '--port', '-p', help="The port to connect to"),
         name: str = typer.Option(..., '--name', '-n', help="name of the team"),
         machine: str = typer.Option("localhost", '--host', '-h', help="name of the machine; localhost by default")):
    """cli for zappy_ai
    """
    if (port < 0 or port > 10000):
        print("invalid port")
        exit(84)
    launch_ai(machine, port, name)

def main_cli():
    """main to call the cli handling
    """
    try:
        typer.run(main_typer)
    except Exception as err:
        print(err.args[0])
        exit(84)

if __name__ == "__main__":
    typer.run(main_cli)
