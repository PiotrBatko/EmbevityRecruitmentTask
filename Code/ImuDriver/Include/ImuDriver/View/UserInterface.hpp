#pragma once

class ImuDriver;

namespace View
{

class UserInterface
{
public:
    enum class Status
    {
        Success,
        UnknownError,
    };

    explicit UserInterface(ImuDriver& imu);
    Status RunMainLoop();

private:
    ImuDriver& m_Imu;
};

}
