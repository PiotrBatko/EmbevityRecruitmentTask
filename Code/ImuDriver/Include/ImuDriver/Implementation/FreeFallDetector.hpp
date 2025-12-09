#pragma once

#include "ImuDriver/Interface/ImuDriver.hpp"

class FreeFallDetector
    : public Interface::ImuDriver::NewDataAcquiredObserver
{
public:
    class FreeFallObserver
    {
    public:
        virtual void OnFreeFallStarted() = 0;
        virtual void OnFreeFallFinished() = 0;

        virtual ~FreeFallObserver() = default;
    };

    void SubscribeToFreeFallDetection(FreeFallObserver& observer);

private:
    void OnNewDataAcquired(float ax, float ay, float az) override;

    bool AccelerationsAreSmall(float ax, float ay, float az);

    bool m_IsFreeFallInProgress = false;
    int m_CurrentFreeFallSamples = 0;
    FreeFallObserver* m_FreeFallObserver = nullptr;
};
