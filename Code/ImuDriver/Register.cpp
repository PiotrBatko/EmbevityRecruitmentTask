#include "ImuDriver/Common/Register.hpp"

#include <format>

namespace Register
{

Address Address::operator++(int)
{
    auto copy{*this};
    ++m_Address;
    return copy;
}

std::string Address::AsString() const
{
    return std::format("0x{:02x}", m_Address);
}

}
