#include "ImuDriver/Common/Logger.hpp"

#include <fstream>

namespace Log
{

namespace
{

constexpr auto DebugLogsEnabled = false;

void CreateLog(const Message& message)
{
    std::ofstream("logs.txt", std::ios::app) << message << std::endl;
}

}

void Debug(const Message& message)
{
    if constexpr (DebugLogsEnabled)
    {
        CreateLog("[DEBUG] " + message);
    }
}

void Info(const Message& message)
{
    CreateLog("[INFO]  " + message);
}

void Error(const Message& message)
{
    CreateLog("[ERROR] " + message);
}

}
