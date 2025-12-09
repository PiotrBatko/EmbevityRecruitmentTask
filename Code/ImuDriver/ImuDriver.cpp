#include "ImuDriver/Implementation/ImuDriver.hpp"

#include "ImuDriver/Implementation/ImuRegisters.hpp"

#include "ImuDriver/Common/BitOperations.hpp"
#include "ImuDriver/Common/Logger.hpp"

#include <utility>

using namespace Common;
using Interface::I2c;

namespace
{

constexpr Register::Value AsRegisterValue(const ImuDriver::AccelerometerScale scale)
{
    using enum ImuDriver::AccelerometerScale;
    switch (scale)
    {
    case Scale16G: return ACCEL_UI_FS_SEL_16G;
    case Scale8G:  return ACCEL_UI_FS_SEL_8G;
    case Scale4G:  return ACCEL_UI_FS_SEL_4G;
    case Scale2G:  return ACCEL_UI_FS_SEL_2G;
    }

    std::unreachable();
}

constexpr Register::Value AsRegisterValue(const ImuDriver::AccelerometerOutputDataRate outputDataRate)
{
    using enum ImuDriver::AccelerometerOutputDataRate;
    switch (outputDataRate)
    {
    case Rate50Hz: return ACCEL_ODR_50HZ;
    case Rate25Hz: return ACCEL_ODR_25HZ;
    }

    std::unreachable();
}

}

ImuDriver::ImuDriver(I2c& i2c, const I2c::SlaveAddress slaveAddress)
    : m_I2c{i2c}
    , m_SlaveAddress{slaveAddress}
{
}

void ImuDriver::SubscribeToNewDataAcquired(NewDataAcquiredObserver& observer)
{
    if (m_NewDataAcquiredObserver)
    {
        Log::Error("Current implementation supports only one subscriber");
        return;
    }

    m_NewDataAcquiredObserver = &observer;
}

ImuDriver::Status ImuDriver::Initialize()
{
    if (auto status = ConfigureAccelerometer(AccelerometerScale::Scale2G, AccelerometerOutputDataRate::Rate50Hz); status != Status::Success)
    {
        return status;
    }

    return Status::Success;
}

ImuDriver::Status ImuDriver::Start()
{
    if (auto status = TurnOnAccelerometerAndGyroscopeInLowNoiseMode(); status != Status::Success)
    {
        return status;
    }

    m_StopSource = std::stop_source{};
    m_DataAcquisitionThread = std::jthread{
        [this]{
            DataAcquisitionThread(m_StopSource.get_token());
        }
    };

    return Status::Success;
}

ImuDriver::Status ImuDriver::Stop()
{
    m_StopSource.request_stop();
    m_DataAcquisitionThread.join();

    if (auto status = TurnOffAccelerometerAndGyroscope(); status != Status::Success)
    {
        return status;
    }

    return Status::Success;
}

bool ImuDriver::IsDataAcquisitionEnabled() const
{
    return m_DataAcquisitionThread.joinable();
}

ImuDriver::Status ImuDriver::ConfigureAccelerometer(const AccelerometerScale scale, const AccelerometerOutputDataRate outputDataRate)
{
    auto [status, acceleratorConfiguration] = m_I2c.ReadByte(m_SlaveAddress, Register::ACCEL_CONFIG0);
    if (status != I2c::Status::Success)
    {
        return Status::UnknownError;
    }

    Bits::Clear(acceleratorConfiguration, ACCEL_UI_FS_SEL_MASK | ACCEL_ODR_MASK);
    Bits::Set(acceleratorConfiguration, AsRegisterValue(scale) | AsRegisterValue(outputDataRate));

    if (m_I2c.WriteByte(m_SlaveAddress, Register::ACCEL_CONFIG0, acceleratorConfiguration) != I2c::Status::Success)
    {
        return Status::UnknownError;
    }

    return Status::Success;
}

void ImuDriver::DataAcquisitionThread(const std::stop_token stopToken)
{
    while (!stopToken.stop_requested())
    {
        const auto result = m_I2c.ReadByte(m_SlaveAddress, Register::INT_STATUS_DRDY);
        if (result.status != I2c::Status::Success)
        {
            Log::Error("Checking if data is acquired failed");
            return;
        }

        const auto isDataReady = Bits::Read(result.readByte, DATA_RDY_INT_MASK) == DATA_RDY_INT_DATA_IS_READY;
        if (not isDataReady)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds{400});
            continue;
        }

        const auto [status, acquiredData] = ReadAllAcquiredData();
        if (status != Status::Success)
        {
            Log::Error("Reading acquired data failed");
            return;
        }

        const auto [ax, ay, az] = ConvertToFloat(acquiredData.acceleration);
        Log::Info(std::format("Received data: ax={: .3f}, ay={: .3f}, az={: .3f}", ax, ay, az));
        if (m_NewDataAcquiredObserver)
        {
            m_NewDataAcquiredObserver->OnNewDataAcquired(ax, ay, az);
        }
    }
}

std::pair<ImuDriver::Status, ImuDriver::AcquiredData> ImuDriver::ReadAllAcquiredData() const
{
    auto result = std::pair<ImuDriver::Status, ImuDriver::AcquiredData>{};
    auto& [status, readData] = result;
    status = Status::UnknownError;

    auto registerToRead = Register::ACCEL_DATA_X1;

    {
        const auto [status, singleData] = ReadSingleAcquiredData(registerToRead);
        if (status != Status::Success)
        {
            return result;
        }
        readData.acceleration[0] = singleData;
    }

    {
        const auto [status, singleData] = ReadSingleAcquiredData(registerToRead);
        if (status != Status::Success)
        {
            return result;
        }
        readData.acceleration[1] = singleData;
    }

    {
        const auto [status, singleData] = ReadSingleAcquiredData(registerToRead);
        if (status != Status::Success)
        {
            return result;
        }
        readData.acceleration[2] = singleData;
    }

    status = Status::Success;
    return result;
}

std::pair<ImuDriver::Status, std::uint16_t> ImuDriver::ReadSingleAcquiredData(Register::Address& target) const
{
    auto result = std::pair<Status, std::uint16_t>{};
    auto& [status, readData] = result;
    status = Status::UnknownError;

    {
        const auto readByteResult = m_I2c.ReadByte(m_SlaveAddress, target++);
        if (readByteResult.status != I2c::Status::Success)
        {
            return result;
        }
        readData = static_cast<std::uint16_t>(readByteResult.readByte) << 8;
    }

    {
        const auto readByteResult = m_I2c.ReadByte(m_SlaveAddress, target++);
        if (readByteResult.status != I2c::Status::Success)
        {
            return result;
        }
        readData |= static_cast<std::uint16_t>(readByteResult.readByte) << 0;
    }

    status = Status::Success;
    return result;
}

std::array<float, 3> ImuDriver::ConvertToFloat(const AcquiredData::Accelerations& acquired) const
{
    return {
        ConvertToFloat(acquired[0]),
        ConvertToFloat(acquired[1]),
        ConvertToFloat(acquired[2])
    };
}

float ImuDriver::ConvertToFloat(const std::uint16_t acquired) const
{
    return static_cast<float>(
        static_cast<std::int16_t>(acquired)
    ) / 16384;
}

ImuDriver::Status ImuDriver::TurnOnAccelerometerAndGyroscopeInLowNoiseMode()
{
    return ConfigurePowerManagement(
        GYRO_MODE_MASK | ACCEL_MODE_MASK,
        GYRO_MODE_ENABLED_LOW_NOISE | ACCEL_MODE_ENABLED_LOW_NOISE
    );
}

ImuDriver::Status ImuDriver::TurnOffAccelerometerAndGyroscope()
{
    return ConfigurePowerManagement(
        GYRO_MODE_MASK | ACCEL_MODE_MASK,
        GYRO_MODE_DISABLED | ACCEL_MODE_DISABLED
    );
}

ImuDriver::Status ImuDriver::ConfigurePowerManagement(const Register::Value mask, const Register::Value bits)
{
    auto [status, powerManagement] = m_I2c.ReadByte(m_SlaveAddress, Register::PWR_MGMT0);
    if (status != I2c::Status::Success)
    {
        return Status::UnknownError;
    }

    Bits::Clear(powerManagement, mask);
    Bits::Set(powerManagement, bits);

    if (m_I2c.WriteByte(m_SlaveAddress, Register::PWR_MGMT0, powerManagement) != I2c::Status::Success)
    {
        return Status::UnknownError;
    }

    return Status::Success;
}
