from trentorian.trentorian import Trantorian
from parser.virtual.parser import IParser
from utils import determine_direction
from parser.concrete.message_type_parser import MessageType

from warnings import warn
from enum import IntEnum
from time import time

class ParamsType(IntEnum):
    """Sound direction enum
    """
    SENDER_UID = 0,
    MESSAGE_UID = 1


class MessageBeaconParser(IParser):
    """class for message of type becon parsing
    """

    def serialize(self, trentorian: Trantorian) -> str:
        """Return the message string for creting a birth info message

        The message returned should ressemble something like that:
            "trentorianUid"
        """
        return f"{trentorian.uid}_{str(time())}"

    def deserialize(self, trentorian: Trantorian, message_content: str, message_hitpoint: int) -> Trantorian:
        """Deserialize the message content, for birth info
        """
        params = message_content.split("_")

        if len(params)!= 2:
            warn(f"Invalid message content for beacon message: {message_content}")
            return trentorian
        if trentorian.state == "beacon":
            try:
                if float(trentorian.uid) < float(params[ParamsType.SENDER_UID]):
                    return trentorian
            except ValueError:
                warn(f"Invalid message content for beacon message: {message_content}")
                return trentorian

        if params[ParamsType.MESSAGE_UID] == trentorian.last_beacon_message_uid:
            trentorian.state = "wander"
            return trentorian

        trentorian.last_beacon_message_uid = params[ParamsType.MESSAGE_UID]

        trentorian.state = "going somewhere"
        trentorian.last_beacon_uid = params[ParamsType.SENDER_UID]
        trentorian.last_beacon_direction = message_hitpoint

        return trentorian
