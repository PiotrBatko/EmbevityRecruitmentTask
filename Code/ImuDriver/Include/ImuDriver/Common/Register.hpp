#pragma once

#include <cstdint>
#include <string>

namespace Register
{

class Address
{
public:
    std::uint8_t m_Address;

    std::string AsString() const;
};

// TODO: Increase type safety with separate class for Register::Value.
using Value = std::uint8_t;

}
