#include "ImuDriver/Implementation/FreeFallLogger.hpp"

#include "ImuDriver/Common/Logger.hpp"

void FreeFallLogger::OnFreeFallStarted()
{
    Log::Info("");
    Log::Info("Free fall started");
    Log::Info("");

}

void FreeFallLogger::OnFreeFallFinished()
{
    Log::Info("");
    Log::Info("Free fall finished");
    Log::Info("");
}
