#pragma once

namespace Interface
{

class ImuDriver
{
public:
    class NewDataAcquiredObserver
    {
    public:
        virtual void OnNewDataAcquired(float ax, float ay, float az) = 0;

        virtual ~NewDataAcquiredObserver() = default;
    };

    virtual void SubscribeToNewDataAcquired(NewDataAcquiredObserver& observer) = 0;

    virtual ~ImuDriver() = default;
};

}
