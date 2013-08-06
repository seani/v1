//
// v1 - Prototype character/item/tradeskill game.
// Copyright (C) 2013 Adam Hayek (adam.hayek@gmail.com)
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see [http://www.gnu.org/licenses/].
//

#include "sim\IFrameRate.h"


//text drawing interface
#include "sim\IScreenText.h"
static IScreenText *screenText;
InterfaceReference(screenText, IScreenText, default);


#define MAX_FRAMES 30
#define TIME_EACH_FRAME 0.25f
#define FRAMES_EACH_SECOND 4

//our implementation of IFrameRate
class FrameRateImpl : public IFrameRate
{
public:
    InterfaceImplementation(FrameRateImpl);

    //from IFrameRate
    bool FrameStatistics(float timeSeconds, Context &caller);

    //data for a frame
    class Frame
    {
    public:
        //how many frames this was worth.
        int numFrames;

        //time spend rendering this frame
        double timeSeconds;
    };

    //all the frames we track right now
    ArrayPointerOwnerQueue<Frame> frames;

    //computed average frame time
    float averageFrameTime;

    //spare objects we can use instead of allocating a new one
    ArrayPointerOwner<Frame> spares;
};

//instantiate and register our implementation
InterfaceCreate(FrameRateImpl, frameRateImpl, IFrameRate, default);

//
//FrameRateImpl functions
//

bool FrameRateImpl::FrameStatistics(float timeSeconds, Context &caller)
{
    CONTEXT_CALLED();

    //where we will save data for this frame
    Frame *frame = (frames.Num() > 0) ? frames.Get(frames.Num() - 1) : null;

    //check if there is room to add this time
    if (frame != null && frame->timeSeconds < TIME_EACH_FRAME)
    {
        //add this time to the frame
        frame->timeSeconds += timeSeconds;

        //add to frame count
        frame->numFrames++;
    }
    else
    {
        //we need a new frame.
        //check if we have a spare
        if (spares.Num() > 0)
        {
            //take one from the spares list
            frame = spares.Remove(0);
        }
        else
        {
            //we need to allocate a new one
            frame = new Frame();
        }

        //add the frame to the list
        frames.Add(frame);

        //save frame data
        frame->timeSeconds = timeSeconds;
        frame->numFrames = 1;
    }

    //check if we have more time remembered than we care about
    while (frames.Num() > MAX_FRAMES)
    {
        //we can remove the oldest frame
        Frame *oldest = frames.Remove(0);

        //add it to our spares
        spares.Add(oldest);
    }

    //total time for all our frames
    float totalTime = 0.0f;

    //total number of frames
    int totalFrameCount = 0;

    //add up all frame times
    for (int i = 0, len = frames.Num(); i < len; i++)
    {
        //add time for this frame
        totalTime += float(frames.Get(i)->timeSeconds);

        //add to total frame count
        totalFrameCount += frames.Get(i)->numFrames;
    }

    //compute frames per second.
    float smoothFps = totalFrameCount / totalTime;

    //reset so we can get a recent fps
    totalTime = 0.0f;
    totalFrameCount = 0;

    //go through last 2 seconds only
    for (int len = frames.Num(), i = max(0, len - FRAMES_EACH_SECOND * 2); i < len; i++)
    {
        //add time for this frame
        totalTime += float(frames.Get(i)->timeSeconds);

        //add to total frame count
        totalFrameCount += frames.Get(i)->numFrames;
    }

    //compute recent fps
    float recentFps = totalFrameCount / totalTime;

    //display the time.
    ubuffer256 frameDisplay; frameDisplay.Set(L"%3.0f FPS (%3.0f)", recentFps, smoothFps);
    IFBREAKCONTEXTMSG(screenText->PrintLineTopLeft(frameDisplay, context) == false, "Error displaying frame rate on screen.");
    
    //success
    return true;
}

