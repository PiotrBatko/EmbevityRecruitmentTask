#pragma once

#include "ImuDriver/Interface/I2c.hpp"

#include "ImuDriver/Implementation/ImuRegisters.hpp"

#include <stop_token>
#include <thread>

class ImuDriver
{
public:
    ImuDriver(Interface::I2c& i2c, Interface::I2c::SlaveAddress slaveAddress);

    void Start();
    void Stop();

private:
    Interface::I2c& m_I2c;
    Interface::I2c::SlaveAddress m_SlaveAddress;
    std::jthread m_DataAcquisitionThread;
    std::stop_source m_StopSource;

    void DataAcquisitionThread(std::stop_token stopToken);
};
