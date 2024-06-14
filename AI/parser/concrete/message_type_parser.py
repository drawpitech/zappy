import importlib
from enum import IntEnum

class MessageType(IntEnum):
    """Message type enum
    """
    ASK_BIRTH = 0
    BIRTH_INFO = 1
    BEACON = 2
    RITUAL_READY = 3

MESSAGE_PARSER: dict = {
    MessageType.ASK_BIRTH: "MessageAskBirthParser",
    MessageType.BIRTH_INFO: "MessageBirthInfoParser",
    MessageType.BEACON: "MessageBeaconParser",
    MessageType.RITUAL_READY: "MessageRitualParser"
}

MODULE_NAME: dict = {
    "MessageBirthInfoParser": "parser.concrete.message_birth_parser",
    "MessageAskBirthParser": "parser.concrete.message_birth_parser",
    "MessageBeaconParser": "parser.concrete.message_beacon_parser",
    "MessageRitualParser": "parser.concrete.message_ritual_parser"
}

class MessageTypeParser():
    """Class for message type parser
    """
    def serialize(self, message_type: MessageType, trentorian):
        """Serialize a message depending on his type
        """
        message_parser: str = MESSAGE_PARSER[message_type]
        module = importlib.import_module(MODULE_NAME[message_parser])
        return str(message_type) + '$' + getattr(module, message_parser)().serialize(trentorian)

    def deserialize(self, trentorian, message_type: MessageType, message_content: str, message_hitpoint: int):
        """Deserialize the message content, depending on his type
        """
        message_parser: str = MESSAGE_PARSER[message_type]
        module = importlib.import_module(MODULE_NAME[message_parser])
        getattr(module, message_parser)().deserialize(trentorian, message_content, message_hitpoint)
        return trentorian
