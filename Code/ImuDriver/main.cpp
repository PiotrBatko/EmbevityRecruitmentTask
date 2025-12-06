#include "ImuDriver/Implementation/ImuRegisters.hpp"
#include "ImuDriver/Implementation/SimulatedI2c.hpp"

using Interface::I2c;

int main(int argc, char *argv[])
{
    auto i2c = SimulatedI2c{"tcp://localhost:5555"};
    const auto slave = I2c::SlaveAddress{0x7F};

    {
        auto result = i2c.WriteByte(slave, Register::ACCEL_CONFIG0, 0x05);
        if (result != I2c::Status::Success)
        {
            return 1;
        }
    }

    {
        auto result = i2c.ReadByte(slave, Register::INT_STATUS_DRDY);
        if (result.status != I2c::Status::Success)
        {
            return 1;
        }

        std::cout << std::format("Received byte: 0x{:02x}", result.readByte) << std::endl;
    }
}
