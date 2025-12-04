#include "ImuDriver/Implementation/ImuRegisters.hpp"
#include "ImuDriver/Implementation/SimulatedI2c.hpp"

int main(int argc, char *argv[])
{
    auto i2c = SimulatedI2c{"tcp://localhost:5555"};
    auto result = i2c.ReadByte(0x7F, Register::INT_STATUS_DRDY);
    if (result.status != Interface::I2c::Status::Success)
    {
        return 1;
    }
    std::cout << std::format("Received byte: 0x{:02x}", result.readByte) << std::endl;
}
