import enum
import traceback
import zmq


class I2cSimulator:
    __START = "START"
    __READ_BIT = "READ_BIT"
    __WRITE_BIT = "WRITE_BIT"
    __ACK = "ACK"
    __NACK = "NACK"
    __STOP = "STOP"

    # What is sent as a response when STOP is requested. In real I2C nothing is
    # sent (communication is over), but sockets requires to send something back.
    __STOP_RESPONSE = "EOF"

    class __State(enum.Enum):
        IDLE = enum.auto()
        WAITING_FOR_ADDRESS_TO_WRITE = enum.auto()
        READING = enum.auto()
        WRITING = enum.auto()

    def __init__(self, address: int):
        self.__state = self.__State.IDLE
        self.__my_address = hex(address)
        self.__selected_register = None

    def process(self, message: bytes) -> str:
        parts = list(message.decode("utf-8").split())
        print(f"Received message: {parts}")

        try:
            if self.__state is self.__State.IDLE:
                assert len(parts) == 3
                assert parts[0] == self.__START, f"Expected \"{self.__START}\", but received \"{parts[0]}\""
                assert parts[1] == self.__my_address, f"Incorrect address, received: \"{parts[1]}\", expected: \"{self.__my_address}\""
                assert parts[2] == self.__WRITE_BIT, f"Expected \"{self.__WRITE_BIT}\", but received \"{parts[2]}\""

                self.__state = self.__State.WAITING_FOR_ADDRESS_TO_WRITE
                return self.__ACK

            if self.__state is self.__State.WAITING_FOR_ADDRESS_TO_WRITE:
                assert len(parts) == 1
                assert parts[0].startswith("0x"), f"Received register for selection: \"{parts[0]}\""

                self.__selected_register = int(parts[0], 16)
                self.__state = self.__State.WRITING
                return self.__ACK

            if self.__state is self.__State.WRITING:
                if len(parts) == 3:
                    assert parts[0] == self.__START, f"Expected \"{self.__START}\", but received \"{parts[0]}\""
                    assert parts[1] == self.__my_address, f"Incorrect address, received: \"{parts[1]}\", expected: \"{self.__my_address}\""
                    assert parts[2] == self.__READ_BIT, f"Expected \"{self.__READ_BIT}\", but received \"{parts[2]}\""

                    self.__state = self.__State.READING
                    read_value = self.get_value_of_register(self.__selected_register)
                    self.__selected_register += 1
                    return f"{self.__ACK} 0x{read_value:02x}"

            if self.__state is self.__State.READING:
                if len(parts) == 2:
                    assert parts[0] == self.__NACK, f"Expected \"{self.__NACK}\", but received \"{parts[0]}\""
                    assert parts[1] == self.__STOP, f"Expected \"{self.__STOP}\", but received \"{parts[1]}\""

                    self.__state = self.__State.IDLE
                    return self.__STOP_RESPONSE

                assert len(parts) == 1
                assert parts[0] == self.__ACK, f"Expected \"{self.__ACK}\", but received \"{parts[0]}\""

                read_value = self.get_value_of_register(self.__selected_register)
                self.__selected_register += 1
                return f"{self.__ACK} 0x{read_value:02x}"

        except Exception as exception:
            traceback.print_exc()
            print(f"Exception: {exception}")

        self.__state = self.__State.IDLE
        self.__selected_register = None
        return "Error: NonImplemented"

    def get_value_of_register(self, register: int | None) -> int:
        return 0xab


def main() -> None:
    context = zmq.Context()
    socket = context.socket(zmq.REP)
    socket.bind("tcp://*:5555")

    i2c = I2cSimulator(address=0x7F)
    while True:
        # Wait for next request from client
        message = socket.recv()

        # Process it
        reply = i2c.process(message)

        # Send reply back to client
        socket.send(reply.encode("utf-8"))
        print(f"Replied: {reply}")


if __name__ == '__main__':
    main()
