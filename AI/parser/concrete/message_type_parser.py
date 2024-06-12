import importlib
from enum import IntEnum

from trentorian import Trantorian

class MessageType(IntEnum):
    """Message type enum
    """
    UPDATE = 0

MESSAGE_PARSER: dict = {
    MessageType.UPDATE: "MessageUpdateParser",
}

MODULE_NAME: dict = {
    "MessageUpdateParser": "message_update_parser",
}

class MessageTypeParser():
    """Class for message type parser
    """
    def serialize(self, message_type: MessageType, trentorian: Trantorian):
        """Serialize a message depending on his type
        """
        message_parser: str = MESSAGE_PARSER[message_type]
        module = importlib.import_module(MODULE_NAME[message_parser])
        return getattr(module, message_parser)().serialize(Trantorian)

    def deserialize(self, trentorian: Trantorian, message_type: MessageType, message_content: str):
        """Deserialize the message content, depending on his type
        """
        message_parser: str = MESSAGE_PARSER[message_type]
        module = importlib.import_module(MODULE_NAME[message_parser])
        getattr(module, message_parser)().deserialize(trentorian, message_content)
        return trentorian
