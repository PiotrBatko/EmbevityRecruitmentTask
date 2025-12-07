#pragma once

class ImuDriver;

namespace View
{

class UserInterface
{
public:
    explicit UserInterface(ImuDriver& imu);
    int RunMainLoop();

private:
    ImuDriver& m_Imu;
};

}
