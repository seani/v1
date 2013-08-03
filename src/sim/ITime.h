#pragma once

#include "common\idb\IBase.h"

//
//Module for keeping track of elapsed time
//

class ITime : public IBase
{
public:
    //initializes timer system
    virtual bool Startup(Context &caller) = 0;

    //call at the beginning of each frame.
    virtual void UpdateTime() = 0;

    //gets the time, in seconds, of the last frame.
    virtual float FrameTime() = 0;

    //gets the time elapsed, in seconds, since init was called.
    virtual float AppTime() = 0;

    //convert the given number of clock cycles to seconds
    virtual float CyclesSeconds(int64 cycleCount) = 0;
};

