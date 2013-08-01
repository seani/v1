#pragma once

class Place;
class UserErrorStore;

#include "common\global\global.h"

//A place in code, usually a function.  
//These objects will normally be statically allocated so they can collect profiling information
//for the entire execution of the program.
class Place
{
    friend class Context;
public:
    inline Place(char const *functionSignature);

private:
    //description of this place
    buffer256 description;

    //number of times this function was called
    int64 numCalls;

    //how many clock cycles were spent in this function
    int64 numCycles;
};

#define HERE() static Place here__(__FUNCSIG__);
#define CONTEXT_ROOT() HERE(); Context context(null, here__);
#define CONTEXT_ROOT_MAKEUSERERROR() HERE(); UserErrorStore _userError; Context context(null, here__, &_userError);
#define CONTEXT_CALLED() HERE(); Context context(&caller, here__);
#define CONTEXT_CALLED_MAKEUSERERROR() HERE(); UserErrorStore _userError; Context context(&caller, here__, &_userError);
#define CONTEXT_CALLED_REQUIREUSERERROR() HERE(); Context context(&caller, here__); IFBREAKCONTEXT(context.UserErrorCapable() == false);
#define CONTEXT_THREAD_ROOT(threadContextPtr) ThreadRootContext *caller = threadContextPtr; AutoDeletePtr<ThreadRootContext> delCaller(caller); HERE(); Context context(caller, here__);
#define IFUSERERROR(expr, _messageParams) if (expr) { context.UserError _messageParams; return true; }
#define CHECKCRASHANDUSERERROR(expr) IFBREAKCONTEXT(expr); if (context.UserErrorOccurred() == true) { return true; }

//part of the chain which describes the full place we are executing.  This object
//will normally be created/deleted as a stack variable which is valid during a function execution.
class Context
{
public:
    inline Context(Context *caller, Place &place);
    inline Context(Context *caller, Place &place, UserErrorStore *userError);
    inline virtual ~Context();

    //save the given crash message and data about where it happened.
    inline void CrashMessage(char const *msg);

    inline const Context *Caller() const;
    inline Place *Spot() const;

    //true if we have the ability to note a user error
    inline bool UserErrorCapable();

    //set the user error message, if we can.  returns true.
    inline bool UserError(wchar const *formatString, ...);

    //true if we have had a user error
    inline bool UserErrorOccurred();

    //get the error message
    inline wchar const *UserErrorMessage();

    //clear the user error state
    inline void UserErrorClear();

    //number of clock cycles spent in our last sub context
    int64 lastCallNumCycles;

protected:
    inline Context();

    //context that encompases us
    Context *caller;

    //where we are executing.
    Place *place;

    //the user error object available to this context, if any
    UserErrorStore *userError;

    //clock cycle when we entered this context
    int64 clockStart;

    //get clock cycle number right now.
    inline void GetClock(int64 &dest);
};

//A context for a user level error such as file not found or incorrect file contents.  Stuff that is not the fault of the programmer.
class UserErrorStore
{
public:
    inline UserErrorStore();

    //true if there has been an error
    bool gotUserError;

    //the error message.
    ubuffer1024 message;
};

//context that can be used as the root of a thread that is created with a built up context in another thread
class ThreadRootContext : public Context
{
public:
    inline ThreadRootContext(Context &threadCreator);

private:
    //call stack of our creator, in reverse order.
    ArrayPointer<Place> stack;
};

//
//ThreadRootContext inline functions
//

ThreadRootContext::ThreadRootContext(Context &threadCreator)
{
    //top of the creator context
    const Context *spot = &threadCreator;

    //go down the call stack
    while (spot != null)
    {
        //check if this spot has a place
        if (spot->Spot() != null)
        {
            //add this spot to our stack
            stack.Add(spot->Spot());
        }

        //go to next spot
        spot = spot->Caller();
    }
}


//
//Place inline functions
//

inline Place::Place(char const *functionSignature)
{
    //unmangle the function name and store it
    //__unDName((char *)&this->description, decoratedFunctionName, (int)sizeof(this->description), UNDNAME_COMPLETE);
    this->description = functionSignature;

    //initialize
    numCalls = 0;
    numCycles = 0;
}


//
//UserError functions
//

inline UserErrorStore::UserErrorStore()
{
    gotUserError = false;
}


//
//Context inline functions
//

inline Context::Context(Context *caller, Place &place)
{
    //save parameters
    this->caller = caller;
    this->place = &place;
    this->lastCallNumCycles = 0;

    //get user error from caller context
    userError = (caller == null) ? null : caller->userError;

    //get current clock cycle
    GetClock(clockStart);
}

inline Context::Context(Context *caller, Place &place, UserErrorStore *userError)
{
    this->caller = caller;
    this->place = &place;
    this->lastCallNumCycles = 0;
    this->userError = userError;

    //get current clock cycle
    GetClock(clockStart);
}

inline Context::Context()
{
    //save parameters
    caller = null;
    place = null;
    lastCallNumCycles = 0;

    //get current clock cycle
    GetClock(clockStart);
}

inline Context::~Context()
{
    //current clock 
    int64 now;

    //get current clock cycle
    GetClock(now);

    //get number of cycles that have elapsed
    int64 elapsedCycles = now - clockStart;

    //add the cycles to the place's total
    place->numCycles += elapsedCycles;

    //increment our place's call count
    place->numCalls++;

    //check if we have a caller
    if (caller != null)
    {
        //set our cycle count into our caller
        caller->lastCallNumCycles = elapsedCycles;
    }
}

inline const Context *Context::Caller() const
{
    return caller;
}

inline Place *Context::Spot() const
{
    return place;
}

inline void Context::CrashMessage(char const *msg)
{
    //log the message...
}

inline void Context::GetClock(int64 &dest)
{
    //get the 2 32-bit words out of the cpu.
    dest = __rdtsc();
}

inline bool Context::UserErrorCapable()
{
    //check if we have error store
    return userError != null;
}

inline bool Context::UserError(wchar const *formatString, ...)
{
    //check if we have a store
    if (userError != null)
    {
        //set flag that we got an error
        userError->gotUserError = true;

        //get our parameter list.
        va_list args;
        va_start(args, formatString);

        //save the message
        userError->message.Sprintf(formatString, args);
    }

    //we always return true
    return true;
}

inline bool Context::UserErrorOccurred()
{
    //check if an error has occurred
    return userError != null && userError->gotUserError == true;
}

inline void Context::UserErrorClear()
{
    //check if we have a store
    if (userError != null)
    {
        //clear error flag
        userError->gotUserError = false;

        //clear the message
        userError->message = L"";
    }
}

inline wchar const *Context::UserErrorMessage()
{
    //check if we have a store
    if (userError != null)
    {
        //get the message
        return userError->message;
    }

    //we have no message
    return L"";
}
