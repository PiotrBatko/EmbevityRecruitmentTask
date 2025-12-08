#pragma once

#include "ImuDriver/Common/Register.hpp"

namespace Common::Bits
{

constexpr void Clear(Register::Value& target, const Register::Value bits)
{
    target &= ~bits;
}

constexpr void Set(Register::Value& target, const Register::Value bits)
{
    target |= bits;
}

constexpr Register::Value Read(const Register::Value& target, const Register::Value bits)
{
    return target & bits;
}

}
