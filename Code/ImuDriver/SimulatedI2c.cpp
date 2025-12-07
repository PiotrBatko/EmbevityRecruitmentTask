#include "ImuDriver/Implementation/SimulatedI2c.hpp"

#include "ImuDriver/Common/Logger.hpp"

#include <format>
#include <sstream>

using Interface::I2c;

namespace Command
{

constexpr auto ReadByte =  std::string{"READ_BYTE"};
constexpr auto WriteByte = std::string{"WRITE_BYTE"};
constexpr auto Error    =  std::string{"ERROR"};

}

SimulatedI2c::SimulatedI2c(const std::string& endpoint)
    : m_Context{}
    , m_Socket{m_Context, zmqpp::socket_type::req}
{
    m_Socket.connect(endpoint);
}

SimulatedI2c::ReadByteResult SimulatedI2c::ReadByte(SlaveAddress, const Register::Address source) const
{
    ReadByteResult result{Status::UnknownError};
    auto response = SendAndReceive(std::format("{} {}", Command::ReadByte, source.AsString()));
    if (response.starts_with(Command::Error))
    {
        return result;
    }

    result.status = Status::Success;
    result.readByte = static_cast<std::uint8_t>(std::stoi(response, nullptr, 16));
    return result;
}

I2c::Status SimulatedI2c::WriteByte(SlaveAddress, Register::Address source, std::uint8_t byte) const
{
    ReadByteResult result{Status::UnknownError};
    auto response = SendAndReceive(std::format("{} {} 0x{:02x}", Command::WriteByte, source.AsString(), byte));
    if (response.starts_with(Command::Error))
    {
        return Status::UnknownError;
    }

    return Status::Success;
}

std::string SimulatedI2c::SendAndReceive(const std::string& toBeSent) const
{
    // Sent the message.
    m_Socket.send(toBeSent);
    Log::Debug(std::format("    Sent: \"{}\"", toBeSent));

    // Receive the response.
    std::string response;
    m_Socket.receive(response);
    Log::Debug(std::format("Received: \"{}\"", response));
    return response;
}
