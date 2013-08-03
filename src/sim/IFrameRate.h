#pragma once

#include "common\idb\IBase.h"

class IFrameRate : public IBase
{
public:
    //give data about frame that was just rendered
    virtual bool FrameStatistics(float timeSeconds, Context &caller) = 0;

    //get the computed average frame time over the recent past
    virtual bool AverageFrameTime(float &timeSeconds, Context &caller) = 0;
};

