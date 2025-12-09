from collections.abc import Iterator
import csv
import itertools
from pathlib import Path
import time
import traceback
import zmq


VERBOSE = False
def debug_print(*args, **kwargs) -> None:
    if VERBOSE:
        print(*args, **kwargs)


class Registers:
    ACCEL_DATA_X1 = 0x0B
    ACCEL_DATA_X0 = 0x0C
    ACCEL_DATA_Y1 = 0x0D
    ACCEL_DATA_Y0 = 0x0E
    ACCEL_DATA_Z1 = 0x0F
    ACCEL_DATA_Z0 = 0x10
    PWR_MGMT0 = 0x1F
    ACCEL_CONFIG0 = 0x21
    INT_STATUS_DRDY = 0x39


class ImuDataProvider:
    Time = float

    def __init__(self, output_data_rate: Time, data_source_path: str | Path):
        self.__last_query = self.__now()
        self.__output_data_rate = output_data_rate
        self.__timepoint_of_last_data_acquisition: ImuDataProvider.Time | None = None
        self.__acquired_data = {
            Registers.ACCEL_DATA_X1: 0xAA,
            Registers.ACCEL_DATA_X0: 0xBB,
            Registers.ACCEL_DATA_Y1: 0xCC,
            Registers.ACCEL_DATA_Y0: 0xDD,
            Registers.ACCEL_DATA_Z1: 0xEE,
            Registers.ACCEL_DATA_Z0: 0xFF,
        }

        def update_data_in_registers() -> Iterator[None]:
            with open(data_source_path, "r", encoding="utf-8") as data_source_file:
                data_source = csv.reader(data_source_file)
                next(data_source)  # Skip header.
                for row in itertools.cycle(data_source):
                    row = [self.__convert_acceleration_to_binary(float(value)) for value in row[:3]]
                    debug_print(f"New acquired data: {row}")
                    self.__acquired_data[Registers.ACCEL_DATA_X1] = int(row[0][:2], 16)
                    self.__acquired_data[Registers.ACCEL_DATA_X0] = int(row[0][2:], 16)
                    self.__acquired_data[Registers.ACCEL_DATA_Y1] = int(row[1][:2], 16)
                    self.__acquired_data[Registers.ACCEL_DATA_Y0] = int(row[1][2:], 16)
                    self.__acquired_data[Registers.ACCEL_DATA_Z1] = int(row[2][:2], 16)
                    self.__acquired_data[Registers.ACCEL_DATA_Z0] = int(row[2][2:], 16)
                    yield

        self.__update_data_in_registers_iterator = update_data_in_registers()

    def __convert_acceleration_to_binary(self, value: float) -> str:
        return f"{(int(value * 16384) & 0xFFFF):04x}"

    def update_data_in_registers(self) -> None:
        next(self.__update_data_in_registers_iterator)

    def is_new_data_ready(self) -> bool:
        """This mimics the behavior of INT_STATUS_DRDY in real IMU device."""

        timepoint_of_next_data_acquisition = self.__timepoint_of_next_data_acquisition()
        is_new_data_acquired = timepoint_of_next_data_acquisition < self.__now()
        if is_new_data_acquired:
            self.update_data_in_registers()
            self.__timepoint_of_last_data_acquisition = timepoint_of_next_data_acquisition
        return is_new_data_acquired

    def get_acquired_data_for_register(self, register: int) -> int:
        return self.__acquired_data[register]

    def __timepoint_of_next_data_acquisition(self) -> Time:
        return self.__timepoint_of_last_data_acquisition + self.__output_data_rate

    def enable(self) -> None:
        self.__timepoint_of_last_data_acquisition = self.__now()
        print("Data acquisition enabled")

    def disable(self) -> None:
        self.__timepoint_of_last_data_acquisition = None
        print("Data acquisition disabled")

    @staticmethod
    def __now() -> Time:
        return time.time()


class ImuSimulator:
    ACCEL_MODE_MASK = 0x03
    ACCEL_MODE_DISABLED = 0x00
    ACCEL_MODE_LOW_NOISE = 0x03

    def __init__(self):
        self.__registers = {
            Registers.PWR_MGMT0: 0x00,
            Registers.ACCEL_CONFIG0: 0x06,
        }
        self.__data_provider = ImuDataProvider(
            0.04,  # for tests: 0.04 seconds, 25 Hz
            "../../TestData/ImuLog.csv"
        )

    def read_from_register(self, register: int) -> int:
        if register in self.__registers:
            return self.__registers[register]
        elif register == Registers.INT_STATUS_DRDY:
            return 0x01 if self.__data_provider.is_new_data_ready() else 0x00
        elif Registers.ACCEL_DATA_X1 <= register <= Registers.ACCEL_DATA_Z0:
            return self.__data_provider.get_acquired_data_for_register(register)
        else:
            return 0xab

    def write_to_register(self, register: int, value: int) -> None:
        if register in self.__registers:
            self.__registers[register] = value

        if register == Registers.ACCEL_CONFIG0:
            print(f"ACCEL_CONFIG0 set to 0x{value:02x}")
        elif register == Registers.PWR_MGMT0:
            print(f"PWR_MGMT0 set to 0x{value:02x}")
            accel_mode_value = value & self.ACCEL_MODE_MASK
            if accel_mode_value == self.ACCEL_MODE_LOW_NOISE:
                self.__data_provider.enable()
            elif accel_mode_value == self.ACCEL_MODE_DISABLED:
                self.__data_provider.disable()
            else:
                raise RuntimeError(f"Unsupported value of ACCEL_MODE (PWR_MGMT0 register) received: 0x{accel_mode_value:02x}")


class I2cSimulator:
    __READ_BYTE = "READ_BYTE"
    __WRITE_BYTE = "WRITE_BYTE"
    __SUCCESS = "SUCCESS"
    __ERROR = "ERROR"

    def __init__(self, imu: ImuSimulator):
        self.__imu = imu

    def process(self, message: bytes) -> str:
        command, *content = list(message.decode("utf-8").split())
        debug_print(f"Received message: [{command}] {content}")

        try:
            if command == self.__READ_BYTE:
                self.__assert_content_has_valid_size(content, 1)
                register_to_read = int(content[0], 16)
                return f"0x{self.__imu.read_from_register(register_to_read):02x}"
            if command == self.__WRITE_BYTE:
                self.__assert_content_has_valid_size(content, 2)
                register_to_write = int(content[0], 16)
                value_to_write = int(content[1], 16)
                self.__imu.write_to_register(register_to_write, value_to_write)
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


def main() -> None:
    context = zmq.Context()
    socket = context.socket(zmq.REP)
    socket.bind("tcp://*:5555")

    i2c = I2cSimulator(ImuSimulator())
    while True:
        # Wait for next request from client
        message = socket.recv()

        # Process it
        reply = i2c.process(message)

        # Send reply back to client
        socket.send(reply.encode("utf-8"))
        debug_print(f"Replied: {reply}")


if __name__ == '__main__':
    main()
