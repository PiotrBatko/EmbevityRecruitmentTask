import traceback
import zmq


class Registers:
    PWR_MGMT0 = 0x1F
    ACCEL_CONFIG0 = 0x21
    INT_STATUS_DRDY = 0x39


class I2cSimulator:
    __READ_BYTE = "READ_BYTE"
    __WRITE_BYTE = "WRITE_BYTE"
    __SUCCESS = "SUCCESS"
    __ERROR = "ERROR"

    def __init__(self):
        self.__registers = {
            Registers.PWR_MGMT0: 0x00,
            Registers.ACCEL_CONFIG0: 0x06,
        }

    def process(self, message: bytes) -> str:
        command, *content = list(message.decode("utf-8").split())
        print(f"Received message: [{command}] {content}")

        try:
            if command == self.__READ_BYTE:
                self.__assert_content_has_valid_size(content, 1)
                register_to_read = int(content[0], 16)
                return f"0x{self.get_value_of_register(register_to_read):02x}"
            if command == self.__WRITE_BYTE:
                self.__assert_content_has_valid_size(content, 2)
                register_to_write = int(content[0], 16)
                value_to_write = int(content[1], 16)
                self.set_value_of_register(register_to_write, value_to_write)
                return self.__SUCCESS
            else:
                return self.__prepare_error_message("Unknown Command")
        except Exception as exception:
            traceback.print_exc()
            print(f"Exception: {exception}")

        return self.__prepare_error_message("Unknown Problem Occurred")

    @staticmethod
    def __assert_content_has_valid_size(content: list[str], expected_size: int) -> None:
        assert len(content) == expected_size, f"Too many elements in the message (expected: {expected_size}, received: {len(content)}): {content}"

    def __prepare_error_message(self, description: str) -> str:
        return f"{self.__ERROR}: {description}"

    def get_value_of_register(self, register: int) -> int:
        if register in self.__registers:
            return self.__registers[register]
        elif register == Registers.INT_STATUS_DRDY:
            return 0xcd
        else:
            return 0xab

    def set_value_of_register(self, register: int, value: int) -> None:
        if register in self.__registers:
            self.__registers[register] = value

        if register == Registers.ACCEL_CONFIG0:
            print(f"ACCEL_CONFIG0 set to 0x{value:02x}")



def main() -> None:
    context = zmq.Context()
    socket = context.socket(zmq.REP)
    socket.bind("tcp://*:5555")

    i2c = I2cSimulator()
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
