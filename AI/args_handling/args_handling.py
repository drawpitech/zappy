#!/usr/bin/env python3
"""
Module providing the argument parsing functions
"""
import argparse

def get_args():
    """Parse the CLI arguments

    Returns:
        Namespace: namespace containing the parsed arguments
    """
    parser = argparse.ArgumentParser(
        usage="%(prog)s\n\t./zappy_ai -p port -n name -he machine", #+ get_usage()
        description="Zappy AI client."
    )

    parser.add_argument(
        "-p",
        type=int,
        metavar='',
        required=True,
        help="port number",
    )
    parser.add_argument(
        "-n",
        type=str,
        metavar='',
        required=True,
        help="name of the team",
    )
    parser.add_argument(
        "-he",
        metavar='',
        type=str,
        default="localhost",
        # required=True,
        help="name of the machine; localhost by default",
    )

    try:
        parsed = parser.parse_args(None, None)
    except:
        exit(84)
    if (parsed.p < 0 or parsed.p > 10000):
        exit(84)
    return parsed

if __name__ == "__main__":
    args = get_args()
    print(args)
