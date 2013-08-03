#include "sim\IFrameRate.h"


//text drawing interface
#include "sim\IScreenText.h"
static IScreenText *screenText;
InterfaceReference(screenText, IScreenText, default);


#define MAX_TIME 20.0f
#define MAX_FRAMES 500

//our implementation of IFrameRate
class FrameRateImpl : public IFrameRate
{
public:
    InterfaceImplementation(FrameRateImpl);

    //from IFrameRate
    bool FrameStatistics(float timeSeconds, Context &caller);
    bool AverageFrameTime(float &timeSeconds, Context &caller);

    //data for a frame
    class Frame
    {
    public:
        //time spend rendering this frame
        float timeSeconds;
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
    Frame *frame = null;

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

    //total time for all our frames
    float totalTime = 0.0f;

    //add up all frame times
    for (int i = 0, len = frames.Num(); i < len; i++)
    {
        //add time for this frame
        totalTime += frames.Get(i)->timeSeconds;
    }

    //time we would have tracked if we remove our oldest frame
    float timeWithoutOldest = totalTime - frames.Get(0)->timeSeconds;

    //check if we have more time remembered than we care about
    while (timeWithoutOldest > MAX_TIME || frames.Num() > MAX_FRAMES)
    {
        //we can remove the oldest frame
        Frame *oldest = frames.Remove(0);

        //add it to our spares
        spares.Add(oldest);

        //remove it's data from our total
        totalTime = timeWithoutOldest;

        //compute time without the new oldest frame.
        timeWithoutOldest = totalTime - frames.Get(0)->timeSeconds;
    }

    //compute average frame time.
    averageFrameTime = totalTime / frames.Num();

    //compute average actualized frame rate
    float currentFps = 1.0f / averageFrameTime;

    //display the time.
    #define ShowLine(setParams) frameDisplay.Set setParams; IFBREAKCONTEXTMSG(screenText->PrintLineTopLeft(frameDisplay, context) == false, "Error displaying frame rate on screen.");
    ubuffer256 frameDisplay;
    ShowLine((L"%8.2f Current FPS", currentFps));
    
    //success
    return true;
}

bool FrameRateImpl::AverageFrameTime(float &frameTime, Context &caller)
{
    CONTEXT_CALLED();

    //save frame time we've computed
    frameTime = averageFrameTime;

    //success
    return true;
}

