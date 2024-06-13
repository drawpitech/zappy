from abc import ABC, abstractmethod
from trentorian.trentorian import Trantorian

class IParser:
    """Interface for the parsers
    """

    @abstractmethod
    def serialize(self, trentorian: Trantorian) -> str:
        """Serialize the data

        Args:
            data (any): data to serialize

        Returns:
            str: serialized data
        """

    @abstractmethod
    def deserialize(self, trentorian: Trantorian, message_content: str) -> Trantorian:
        """Deserialize the data

        Args:
            data (str): data to deserialize

        Returns:
            any: deserialized data
        """
