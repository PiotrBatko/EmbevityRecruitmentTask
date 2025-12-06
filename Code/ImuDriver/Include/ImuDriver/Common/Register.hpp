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

}
