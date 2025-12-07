#include "ImuDriver/Implementation/ImuDriver.hpp"
#include "ImuDriver/Implementation/SimulatedI2c.hpp"

#include "ImuDriver/View/UserInterface.hpp"

using Interface::I2c;

int main(int argc, char *argv[])
{
    auto i2c = SimulatedI2c{"tcp://localhost:5555"};
    const auto slave = I2c::SlaveAddress{0x7F};

    auto imu = ImuDriver{i2c, slave};

    auto userInterface = View::UserInterface{imu};
    return userInterface.RunMainLoop();
}
