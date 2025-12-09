#pragma once

#include "ImuDriver/Implementation/FreeFallDetector.hpp"

class FreeFallLogger
    : public FreeFallDetector::FreeFallObserver
{
private:
    void OnFreeFallStarted() override;
    void OnFreeFallFinished() override;
};
