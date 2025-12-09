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

    struct AcquiredData
    {
        using Accelerations = std::array<std::uint16_t, 3>;

        Accelerations acceleration;
        // std::array<std::uint16_t, 3> rotation;
    };
    std::pair<Status, AcquiredData> ReadAllAcquiredData() const;
    std::pair<Status, std::uint16_t> ReadSingleAcquiredData(Register::Address& target) const;

    std::array<float, 3> ConvertToFloat(const AcquiredData::Accelerations& acquired) const;
    float ConvertToFloat(std::uint16_t acquired) const;

    Status TurnOnAccelerometerAndGyroscopeInLowNoiseMode();
    Status TurnOffAccelerometerAndGyroscope();

    Status ConfigurePowerManagement(Register::Value mask, Register::Value bits);
};
