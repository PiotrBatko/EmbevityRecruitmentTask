#include "ImuDriver/Implementation/ImuDriver.hpp"

#include "ImuDriver/Common/Logger.hpp"

using Interface::I2c;

ImuDriver::ImuDriver(I2c& i2c, const I2c::SlaveAddress slaveAddress)
    : m_I2c{i2c}
    , m_SlaveAddress{slaveAddress}
{
}

void ImuDriver::Start()
{
    m_StopSource = std::stop_source{};
    m_DataAcquisitionThread = std::jthread{
        [this]{
            DataAcquisitionThread(m_StopSource.get_token());
        }
    };
}

void ImuDriver::Stop()
{
    m_StopSource.request_stop();
    m_DataAcquisitionThread.join();
}

bool ImuDriver::IsDataAcquisitionEnabled() const
{
    return m_DataAcquisitionThread.joinable();
}

void ImuDriver::DataAcquisitionThread(const std::stop_token stopToken)
{
    while (!stopToken.stop_requested())
    {
        const auto result = m_I2c.ReadByte(m_SlaveAddress, Register::INT_STATUS_DRDY);
        if (result.status != I2c::Status::Success)
        {
            return;
        }

        Log::Debug(std::format("Received byte: 0x{:02x}", result.readByte));

        std::this_thread::sleep_for(std::chrono::seconds{3});
    }
}
