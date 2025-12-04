#include "ImuDriver/Implementation/SimulatedI2c.hpp"

#include <format>
#include <sstream>

using Interface::I2c;

//#define HIDE_DEBUG_LOGS
#ifndef HIDE_DEBUG_LOGS
#include <iostream>

#define DEBUG_LOG(message) std::cout << message << std::endl;
#else
#define DEBUG_LOG(message) (void) message;
#endif

SimulatedI2c::SimulatedI2c(const std::string& endpoint)
    : m_Context{}
    , m_Socket{m_Context, zmqpp::socket_type::req}
{
    m_Socket.connect(endpoint);
}

SimulatedI2c::ReadByteResult SimulatedI2c::ReadByte(const SlaveAddress slave, const Register::Address source)
{
    ReadByteResult result{Status::UnknownError};
    if (StartWrite(slave) != Status::Success)
    {
        return result;
    }

    if (SelectRegister(source) != Status::Success)
    {
        return result;
    }

    const auto [status, readByte] = StartRead(slave);
    if (status != Status::Success)
    {
        return result;
    }

    if (StopCommunication() != Status::Success)
    {
        return result;
    }

    result.status = Status::Success;
    result.readByte = readByte;
    return result;
}

I2c::Status SimulatedI2c::StartWrite(const SlaveAddress slave)
{
    if (SendAndReceive(std::format("{} 0x{:02x} {}", Start, slave, WriteBit)) != Ack)
    {
        return Status::UnknownError;
    }

    return Status::Success;
}

std::pair<I2c::Status, std::uint8_t> SimulatedI2c::StartRead(const SlaveAddress slave)
{
    auto response = std::istringstream{SendAndReceive(std::format("{} 0x{:02x} {}", Start, slave, ReadBit))};

    auto expectedAck = std::string{};
    response >> expectedAck;
    if (expectedAck != Ack)
    {
        return {Status::UnknownError, 0};
    }

    auto readByte = int{};
    response >> std::hex >> readByte;
    return {Status::Success, static_cast<std::uint8_t>(readByte)};
}

I2c::Status SimulatedI2c::SelectRegister(Register::Address slaveRegister)
{
    if (SendAndReceive(slaveRegister.AsString()) != Ack)
    {
        return Status::UnknownError;
    }

    return Status::Success;
}

I2c::Status SimulatedI2c::StopCommunication()
{
    if (SendAndReceive(std::format("{} {}", Nack, Stop)) != Eof)
    {
        return Status::UnknownError;
    }

    return Status::Success;
}

std::string SimulatedI2c::SendAndReceive(const std::string& toBeSent)
{
    // Sent the message.
    m_Socket.send(toBeSent);
    DEBUG_LOG(std::format("    Sent: \"{}\"", toBeSent));

    // Receive the response.
    std::string response;
    m_Socket.receive(response);
    DEBUG_LOG(std::format("Received: \"{}\"", response));
    return response;
}
