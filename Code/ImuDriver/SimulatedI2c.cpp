#include "ImuDriver/Implementation/SimulatedI2c.hpp"

#include "ImuDriver/Common/Logger.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

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
    : m_Socket{socket(AF_INET, SOCK_STREAM, 0)}
{
    if (m_Socket == -1) throw std::runtime_error{"Error occurred during socket creation"};

    auto hint = sockaddr_in{};
    hint.sin_family = AF_INET;
    hint.sin_port = htons(5555);
    inet_pton(AF_INET, "127.0.0.1", &hint.sin_addr);

    if (connect(m_Socket, (sockaddr*)&hint, sizeof(hint)) == -1) throw std::runtime_error{"Error during connection creation"};
}

SimulatedI2c::~SimulatedI2c()
{
    close(m_Socket);
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
    if (send(m_Socket, toBeSent.c_str(), toBeSent.size() + 1, 0) == -1) throw std::runtime_error{"Sending error"};
    Log::Debug(std::format("    Sent: \"{}\"", toBeSent));

    // Receive the response.
    auto response = std::array<char, 256>{};
    auto bytesReceived = recv(m_Socket, response.data(), response.size(), 0);
    if (bytesReceived == -1) throw std::runtime_error{"Receiving error"};
    Log::Debug(std::format("Received: \"{}\"", response));
    return response.data();
}
