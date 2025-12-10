#pragma once

#include "ImuDriver/Interface/I2c.hpp"

#include <string>

class SimulatedI2c
    : public Interface::I2c
{
public:
    SimulatedI2c(const std::string& endpoint);
    ~SimulatedI2c();

    [[nodiscard]] ReadByteResult ReadByte(SlaveAddress slave, Register::Address source) const override;
    [[nodiscard]] Status WriteByte(SlaveAddress slave, Register::Address source, std::uint8_t byte) const override;

private:
    int m_Socket;

    std::string SendAndReceive(const std::string& toBeSent) const;
};
