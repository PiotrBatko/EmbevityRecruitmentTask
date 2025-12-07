#pragma once

#include "ImuDriver/Interface/I2c.hpp"

#include <stop_token>
#include <thread>

class ImuDriver
{
public:
    enum class Status
    {
        Success,
        UnknownError,
    };

    ImuDriver(Interface::I2c& i2c, Interface::I2c::SlaveAddress slaveAddress);

    Status Initialize();

    Status Start();
    Status Stop();
    bool IsDataAcquisitionEnabled() const;

    enum class AccelerometerScale
    {
        Scale16G,
        Scale8G,
        Scale4G,
        Scale2G,
    };

    enum class AccelerometerOutputDataRate
    {
        Rate50Hz,
        Rate25Hz,
    };

    Status ConfigureAccelerometer(AccelerometerScale scale, AccelerometerOutputDataRate outputDataRate);

private:
    Interface::I2c& m_I2c;
    Interface::I2c::SlaveAddress m_SlaveAddress;
    std::jthread m_DataAcquisitionThread;
    std::stop_source m_StopSource;

    void DataAcquisitionThread(std::stop_token stopToken);

    Status TurnOnAccelerometerAndGyroscopeInLowNoiseMode();
    Status TurnOffAccelerometerAndGyroscope();

    Status ConfigurePowerManagement(Register::Value mask, Register::Value bits);
};
