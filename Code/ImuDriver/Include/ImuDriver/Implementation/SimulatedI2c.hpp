#pragma once

#include "ImuDriver/Interface/I2c.hpp"

#include <zmqpp/zmqpp.hpp>

#include <string>

class SimulatedI2c
    : public Interface::I2c
{
public:
    SimulatedI2c(const std::string& endpoint);

    [[nodiscard]] ReadByteResult ReadByte(SlaveAddress slave, Register::Address source) override;

private:
    zmqpp::context m_Context;
    zmqpp::socket m_Socket;

    static constexpr auto Start    = std::string{"START"};
    static constexpr auto Stop     = std::string{"STOP"};
    static constexpr auto WriteBit = std::string{"WRITE_BIT"};
    static constexpr auto ReadBit  = std::string{"READ_BIT"};
    static constexpr auto Ack      = std::string{"ACK"};
    static constexpr auto Nack     = std::string{"NACK"};
    static constexpr auto Eof      = std::string{"EOF"};

    /// Sends:
    /// - START,
    /// - Slave address,
    /// - Write bit.
    /// Expects answer:
    /// - ACK.
    [[nodiscard]] Status StartWrite(SlaveAddress slave);

    /// Sends:
    /// - START,
    /// - Slave address,
    /// - Read bit.
    /// Expects answer:
    /// - ACK,
    /// - One read byte.
    [[nodiscard]] std::pair<Status, std::uint8_t> StartRead(SlaveAddress slave);

    /// Sends:
    /// - Address of slave's register to be selected.
    /// Expects answer:
    /// - ACK.
    [[nodiscard]] Status SelectRegister(Register::Address slaveRegister);

    /// Sends:
    /// - NACK,
    /// - STOP,
    /// Expects answer:
    /// - EOF - special message not present in real I2C, but in socket model
    ///         something have to be replied).
    [[nodiscard]] Status StopCommunication();

    std::string SendAndReceive(const std::string& toBeSent);
};
