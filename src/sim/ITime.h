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

