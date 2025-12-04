#include "ImuDriver/Common/Register.hpp"

#include <format>

namespace Register
{

std::string Address::AsString() const
{
    return std::format("0x{:02x}", m_Address);
}

}
