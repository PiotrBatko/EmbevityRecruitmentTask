#pragma once

#include "ImuDriver/Common/Register.hpp"

namespace Register
{

constexpr auto ACCEL_DATA_X1 = Address{0x0B};
constexpr auto ACCEL_DATA_X0 = Address{0x0C};
constexpr auto ACCEL_DATA_Y1 = Address{0x0D};
constexpr auto ACCEL_DATA_Y0 = Address{0x0E};
constexpr auto ACCEL_DATA_Z1 = Address{0x0F};
constexpr auto ACCEL_DATA_Z0 = Address{0x10};

constexpr auto PWR_MGMT0 = Address{0x1F};
constexpr auto ACCEL_CONFIG0 = Address{0x21};
constexpr auto INT_STATUS_DRDY = Address{0x39};

}
