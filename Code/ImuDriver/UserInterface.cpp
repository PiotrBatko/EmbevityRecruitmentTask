#include "ImuDriver/View/UserInterface.hpp"

#include "ImuDriver/Common/Logger.hpp"

#include "ImuDriver/Implementation/ImuDriver.hpp"

#include <filesystem>
#include <format>
#include <iostream>

namespace View
{

namespace Command
{

constexpr auto StartAcquisition = std::string{"start"};
constexpr auto StopAcquisition = std::string{"stop"};
constexpr auto Exit = std::string{"exit"};

}

UserInterface::UserInterface(ImuDriver& imu)
    : m_Imu{imu}
{
}

UserInterface::Status UserInterface::RunMainLoop()
{
    std::cout << "Run following command in separate terminal to track logs:" << std::endl;
    std::cout << "tail -f " << std::filesystem::current_path() / Log::FileName << std::endl;
    std::cout << std::endl;

    Log::Info("Application started");

    while (true)
    {
        std::cout << std::format("Type one of following options:") << std::endl;
        std::cout << std::endl;
        if (m_Imu.IsDataAcquisitionEnabled())
        {
            std::cout << std::format("{} -- to TURN OFF data acquisition", Command::StopAcquisition) << std::endl;
        }
        else
        {
            std::cout << std::format("{} -- to TURN ON data acquisition", Command::StartAcquisition) << std::endl;
        }
        std::cout << std::format("{} -- to close the application", Command::Exit) << std::endl;
        std::cout << std::endl;
        std::cout << "What to do: " << std::flush;

        auto input = std::string{};
        std::getline(std::cin, input);

        Log::Debug(std::format("Input: \"{}\"", input));

        if (input == Command::StartAcquisition and not m_Imu.IsDataAcquisitionEnabled())
        {
            m_Imu.Start();
        }
        else if (input == Command::StopAcquisition and m_Imu.IsDataAcquisitionEnabled())
        {
            m_Imu.Stop();
        }
        else if (input == Command::Exit)
        {
            break;
        }
        else
        {
            std::cout << std::endl;
            std::cout << "Error: Incorrect input, please try again..." << std::endl;
            std::cout << std::endl;
        }
    }

    Log::Info("Application closed");
    return Status::Success;
}

}
