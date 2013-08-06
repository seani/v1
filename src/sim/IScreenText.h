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

