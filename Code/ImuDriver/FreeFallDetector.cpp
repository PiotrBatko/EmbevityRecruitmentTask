#include "ImuDriver/Implementation/FreeFallDetector.hpp"

#include "ImuDriver/Common/Logger.hpp"

#include <cmath>

namespace
{

constexpr auto SmallAccelerationLimit = 0.2;
constexpr auto NumberOfSamplesToDetectFreeFall = 8;

}

void FreeFallDetector::SubscribeToFreeFallDetection(FreeFallObserver& observer)
{
    if (m_FreeFallObserver)
    {
        Log::Error("Current implementation supports only one subscriber");
        return;
    }

    m_FreeFallObserver = &observer;
}

void FreeFallDetector::OnNewDataAcquired(const float ax, const float ay, const float az)
{
    const bool accelerationsAreSmall = AccelerationsAreSmall(ax, ay, az);
    if (not accelerationsAreSmall)
    {
        if (m_IsFreeFallInProgress and m_FreeFallObserver)
        {
            m_FreeFallObserver->OnFreeFallFinished();
        }

        m_IsFreeFallInProgress = false;
        m_CurrentFreeFallSamples = 0;
        return;
    }

    m_CurrentFreeFallSamples += accelerationsAreSmall ? 1 : 0;
    if (not m_IsFreeFallInProgress and m_CurrentFreeFallSamples >= NumberOfSamplesToDetectFreeFall)
    {
        if (m_FreeFallObserver)
        {
            m_FreeFallObserver->OnFreeFallStarted();
        }

        m_IsFreeFallInProgress = true;
    }
}

bool FreeFallDetector::AccelerationsAreSmall(const float ax, const float ay, const float az)
{
    constexpr auto limit = SmallAccelerationLimit;
    return std::abs(ax) < limit and std::abs(ay) < limit and std::abs(az) < limit;
}
