#include "ImuDriver/Implementation/FreeFallDetector.hpp"
#include "ImuDriver/Implementation/FreeFallLogger.hpp"
#include "ImuDriver/Implementation/ImuDriver.hpp"
#include "ImuDriver/Implementation/SimulatedI2c.hpp"

#include "ImuDriver/View/UserInterface.hpp"

using Interface::I2c;

int main(int argc, char *argv[])
{
    auto i2c = SimulatedI2c{""};
    const auto slave = I2c::SlaveAddress{0x7F};

    auto imu = ImuDriver{i2c, slave};

    auto freeFallDetector = FreeFallDetector{};
    imu.SubscribeToNewDataAcquired(freeFallDetector);

    auto freeFallLogger = FreeFallLogger{};
    freeFallDetector.SubscribeToFreeFallDetection(freeFallLogger);

    auto userInterface = View::UserInterface{imu};
    return static_cast<int>(userInterface.RunMainLoop());
}
