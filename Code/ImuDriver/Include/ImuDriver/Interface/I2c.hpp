#pragma once

#include "ImuDriver/Common/Register.hpp"

#include <cstdint>

namespace Interface
{

class I2c
{
public:
    using SlaveAddress = std::uint8_t;

    enum class Status
    {
        Success,
        UnknownError,
    };

    struct ReadByteResult
    {
        Status status;
        std::uint8_t readByte;
    };

    [[nodiscard]] virtual ReadByteResult ReadByte(SlaveAddress slave, Register::Address source) const = 0;
    [[nodiscard]] virtual Status WriteByte(SlaveAddress slave, Register::Address source, std::uint8_t byte) const = 0;

    virtual ~I2c() = default;
};

}
