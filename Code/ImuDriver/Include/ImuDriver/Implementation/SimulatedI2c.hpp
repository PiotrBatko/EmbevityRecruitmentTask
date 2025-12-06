#pragma once

#include "ImuDriver/Interface/I2c.hpp"

#include <zmqpp/zmqpp.hpp>

#include <string>

class SimulatedI2c
    : public Interface::I2c
{
public:
    SimulatedI2c(const std::string& endpoint);

    [[nodiscard]] ReadByteResult ReadByte(SlaveAddress slave, Register::Address source) const override;
    [[nodiscard]] Status WriteByte(SlaveAddress slave, Register::Address source, std::uint8_t byte) const override;

private:
    zmqpp::context m_Context;
    mutable zmqpp::socket m_Socket;

    std::string SendAndReceive(const std::string& toBeSent) const;
};
