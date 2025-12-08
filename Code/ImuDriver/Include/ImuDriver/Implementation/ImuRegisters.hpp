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

// ACCEL_CONFIG0
constexpr auto ACCEL_UI_FS_SEL_OFFSET = 5;
constexpr auto ACCEL_ODR_OFFSET       = 0;

constexpr auto ACCEL_UI_FS_SEL_MASK = std::uint8_t{0x03 << ACCEL_UI_FS_SEL_OFFSET};
constexpr auto ACCEL_ODR_MASK       = std::uint8_t{0x0F << ACCEL_ODR_OFFSET};

constexpr auto ACCEL_UI_FS_SEL_16G = std::uint8_t{0x00 << ACCEL_UI_FS_SEL_OFFSET};
constexpr auto ACCEL_UI_FS_SEL_8G  = std::uint8_t{0x01 << ACCEL_UI_FS_SEL_OFFSET};
constexpr auto ACCEL_UI_FS_SEL_4G  = std::uint8_t{0x02 << ACCEL_UI_FS_SEL_OFFSET};
constexpr auto ACCEL_UI_FS_SEL_2G  = std::uint8_t{0x03 << ACCEL_UI_FS_SEL_OFFSET};

constexpr auto ACCEL_ODR_50HZ = std::uint8_t{0x0A << ACCEL_ODR_OFFSET};
constexpr auto ACCEL_ODR_25HZ = std::uint8_t{0x0B << ACCEL_ODR_OFFSET};

// PWR_MGMT0
constexpr auto GYRO_MODE_OFFSET  = 2;
constexpr auto ACCEL_MODE_OFFSET = 0;

constexpr auto GYRO_MODE_MASK  = std::uint8_t{0x03 << GYRO_MODE_OFFSET};
constexpr auto ACCEL_MODE_MASK = std::uint8_t{0x03 << ACCEL_MODE_OFFSET};

constexpr auto GYRO_MODE_DISABLED          = std::uint8_t{0x00 << GYRO_MODE_OFFSET};
constexpr auto GYRO_MODE_ENABLED_LOW_NOISE = std::uint8_t{0x03 << GYRO_MODE_OFFSET};

constexpr auto ACCEL_MODE_DISABLED          = std::uint8_t{0x00 << ACCEL_MODE_OFFSET};
constexpr auto ACCEL_MODE_ENABLED_LOW_NOISE = std::uint8_t{0x03 << ACCEL_MODE_OFFSET};

// INT_STATUS_DRDY
constexpr auto DATA_RDY_INT_OFFSET = 0;

constexpr auto DATA_RDY_INT_MASK = std::uint8_t{0x01 << DATA_RDY_INT_OFFSET};

constexpr auto DATA_RDY_INT_DATA_IS_READY = std::uint8_t{0x01 << DATA_RDY_INT_OFFSET};
