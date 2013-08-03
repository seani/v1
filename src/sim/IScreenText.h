#pragma once

#include "common\idb\IBase.h"

namespace sf
{
    class RenderWindow;
}

class IScreenText : public IBase
{
public:
    //initialize screen text drawing system
    virtual bool Startup(Context &caller) = 0;

    //print a line of text that goes under other lines on the left of the screen, starting from the top.
    virtual bool PrintLineTopLeft(wchar const *text, Context &caller) = 0;

    //draw all our text to the given window
    virtual bool RenderText(sf::RenderWindow *window, Context &caller) = 0;

};

