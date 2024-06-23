"""Handling of the messages parsing"""

import importlib
from enum import IntEnum

class MessageType(IntEnum):
    """Message type enum
    """
    ASK_BIRTH = 0
    BIRTH_INFO = 1
    BEACON = 2
    RITUAL_READY = 3
    RITUAL_FINISH = 4
    RITUAL_FAILED = 5

MESSAGE_PARSER: dict = {
    MessageType.ASK_BIRTH: "MessageAskBirthParser",
    MessageType.BIRTH_INFO: "MessageBirthInfoParser",
    MessageType.BEACON: "MessageBeaconParser",
    MessageType.RITUAL_READY: "MessageRitualParser",
    MessageType.RITUAL_FINISH: "MessageRitualFinishParser",
    MessageType.RITUAL_FAILED: "MessageRitualFailedParser"
}

MODULE_NAME: dict = {
    "MessageBirthInfoParser": "zappy_ai.parser.concrete.message_birth_parser",
    "MessageAskBirthParser": "zappy_ai.parser.concrete.message_birth_parser",
    "MessageBeaconParser": "zappy_ai.parser.concrete.message_beacon_parser",
    "MessageRitualParser": "zappy_ai.parser.concrete.message_ritual_parser",
    "MessageRitualFinishParser": "zappy_ai.parser.concrete.message_ritual_parser",
    "MessageRitualFailedParser": "zappy_ai.parser.concrete.message_ritual_parser"
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
