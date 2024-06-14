from trentorian.trentorian import Trantorian
from parser.virtual.parser import IParser
from utils import determine_direction
from parser.concrete.message_type_parser import MessageType

from warnings import warn
from enum import IntEnum

class ParamsType(IntEnum):
    """Sound direction enum
    """
    UID = 0


class MessageBeaconParser(IParser):
    """class for message of type becon parsing
    """

    def serialize(self, trentorian: Trantorian) -> str:
        """Return the message string for creting a birth info message

        The message returned should ressemble something like that:
            "senderx_sendery_reiceiverx_reiceivery_teamSize"
        """
        return f"{trentorian.uid}"

    def deserialize(self, trentorian: Trantorian, message_content: str, message_hitpoint: int) -> Trantorian:
        """Deserialize the message content, for birth info
        """
        if trentorian.state == "beacon":
            try:
                if float(trentorian.uid) < float(message_content):
                    return trentorian
            except ValueError:
                warn(f"Invalid message content for beacon message: {message_content}")
                return trentorian

        trentorian.state = "going somewhere"
        trentorian.last_beacon_direction = message_hitpoint

        return trentorian
