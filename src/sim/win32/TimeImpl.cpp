#include "pch.h"
#include "sim\ITime.h"


//ITime implementation
class TimeImpl : public ITime
{
public:
    InterfaceImplementation(TimeImpl);

    TimeImpl();

    //ITime functions.
    bool Startup(Context &caller);
    void UpdateTime();
    float FrameTime();
    float AppTime();
    float CyclesSeconds(int64 cycleCount);

    //cpu tick when timer was initialized
    int64 startupCpuTick;

    //high res timer tick when initialized
    int64 startupHighresTick;

    //high res timer frequency
    int64 highresTicksPerSecond;

    //cpu frequency
    int64 cpuTicksPerSecond;

    //time of last query.
    int64 lastFrameHighresTick;

    //how much time went by in last frame, in seconds.
    float lastFrameTimeSeconds;

    //how much time has gone by since first init call.
    float appTimeSeconds;

    //true if we are done computing cpu speed.
    bool haveComputedSpeed;

    //
    //Worker functions
    //

    //gets the initial tick and clock values
    void InitBegin();
    void InitUpdate();
};

//instantiate our implementation
InterfaceCreate(TimeImpl, timeInst, ITime, default);


//
//TimeImpl functions
//

TimeImpl::TimeImpl()
{

}

bool TimeImpl::Startup(Context &caller)
{
    CONTEXT_CALLED();

    //we need to compute our tick frequency
    haveComputedSpeed = false;

    //get high res timer frequency
    IFBREAKFALSE(QueryPerformanceFrequency((LARGE_INTEGER *)&highresTicksPerSecond) == 0);

    //get current high res tick
    IFBREAKFALSE(QueryPerformanceCounter((LARGE_INTEGER *)&startupHighresTick) == 0);

    //get current cpu tick
    startupCpuTick = __rdtsc();

    //start our app time
    appTimeSeconds = 0.0f;

    //reset frame time
    lastFrameHighresTick = startupHighresTick;
    lastFrameTimeSeconds = 0.0f;

    //guess at cpu ticks per second
    cpuTicksPerSecond = 2800000000;

    //success
    return true;
}

void TimeImpl::InitUpdate()
{
    //current high res tick
    int64 currentHighresTick;

    //get current high res tick
    QueryPerformanceCounter((LARGE_INTEGER *)&currentHighresTick);

    //compute time diff of high res timer.
    int64 highResTicks = currentHighresTick - startupHighresTick;

    //check if there has been any time difference in our high res timer
    if (highResTicks > 0)
    {
        //get current cpu tick
        int64 currentCpuTick = __rdtsc();

        //compute how many seconds have passed
        double seconds = double(highResTicks) / double(highresTicksPerSecond);

        //compute how many cpu ticks per second
        cpuTicksPerSecond = int64((currentCpuTick - startupCpuTick) / seconds);

        //check if enough time has passed that we are confident
        if (seconds > 20.0f)
        {
            //we're done computing speed
            haveComputedSpeed = true;
        }
    }
}

void TimeImpl::UpdateTime()
{
    //check if we have finished computing cpu speed
    if (haveComputedSpeed == false)
    {
        //Work on computing the ticks per second
        InitUpdate();
    }

    //current high res tick
    int64 currentHighresTick;

    //get current high res tick
    QueryPerformanceCounter((LARGE_INTEGER *)&currentHighresTick);

    //compute how many seconds since last update
    lastFrameTimeSeconds = float(currentHighresTick - lastFrameHighresTick) / highresTicksPerSecond;

    //compute how many seconds since startup time
    appTimeSeconds = float(currentHighresTick - startupHighresTick) / highresTicksPerSecond;

    //remember current tick for next time
    lastFrameHighresTick = currentHighresTick;
}

float TimeImpl::FrameTime()
{
    //return the time we computed
    return lastFrameTimeSeconds;
}

float TimeImpl::AppTime()
{
    //return time we computed
    return appTimeSeconds;
}

float TimeImpl::CyclesSeconds(int64 cycleCount)
{
    //divide by number of cycles per second
    return cycleCount / float(cpuTicksPerSecond);
}

