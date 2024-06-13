import importlib
from enum import IntEnum

class MessageType(IntEnum):
    """Message type enum
    """
    ASK_BIRTH = 0
    BIRTH_INFO = 1

MESSAGE_PARSER: dict = {
    MessageType.ASK_BIRTH: "MessageAskBirthParser",
    MessageType.BIRTH_INFO: "MessageBirthInfoParser"
}

MODULE_NAME: dict = {
    "MessageBirthInfoParser": "parser.concrete.message_birth_parser",
    "MessageAskBirthParser": "parser.concrete.message_birth_parser"
}

class MessageTypeParser():
    """Class for message type parser
    """
    def serialize(self, message_type: MessageType, trentorian):
        """Serialize a message depending on his type
        """
        message_parser: str = MESSAGE_PARSER[message_type]
        module = importlib.import_module(MODULE_NAME[message_parser])
        return getattr(module, message_parser)().serialize(trentorian)

    def deserialize(self, trentorian, message_type: MessageType, message_content: str):
        """Deserialize the message content, depending on his type
        """
        message_parser: str = MESSAGE_PARSER[message_type]
        module = importlib.import_module(MODULE_NAME[message_parser])
        getattr(module, message_parser)().deserialize(trentorian, message_content)
        return trentorian
