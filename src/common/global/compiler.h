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

//
//One of our compiler id's must be defined
//

#if defined(__COMPILER_MSVC)
#else
    #error One of [ __COMPILER_MSVC ] compiler id's must be defined.
#endif

//
//One of the release configuration id's must be defined
//

#if defined(__CONFIG_DEBUG)
    #if defined(__CONFIG_RELEASE) || defined(__CONFIG_FINAL)
        #error Only one of [ __CONFIG_DEBUG | __CONFIG_DEBUG | __CONFIG_FINAL ] configuration id's may be defined.
    #endif
#elif defined(__CONFIG_RELEASE)
    #if defined(__CONFIG_DEBUG) || defined(__CONFIG_FINAL)
        #error Only one of [ __CONFIG_DEBUG | __CONFIG_DEBUG | __CONFIG_FINAL ] configuration id's may be defined.
    #endif
#elif defined(__CONFIG_FINAL)
    #if defined(__CONFIG_DEBUG) || defined(__CONFIG_DEBUG)
        #error Only one of [ __CONFIG_DEBUG | __CONFIG_DEBUG | __CONFIG_FINAL ] configuration id's may be defined.
    #endif
#else
    #error One of [ __CONFIG_DEBUG | __CONFIG_RELEASE | __CONFIG_FINAL ] configuration id's must be defined.
#endif

//
//One of the platform id's must be defined
//

#if defined(__PLATFORM_WIN32_PC)
#else
    #error One of [ __PLATFORM_WIN32_PC ] platform id's must be defined.
#endif


//
//
//do compiler specific things.
//
//

#ifdef __COMPILER_MSVC

//turn off dumb warnings
#pragma warning(disable: 4710) //function '' not inlined
#pragma warning(disable: 4514) //'' : unreference inline function has been removed
#pragma warning(disable: 4100) //'' : unreference formal parameter
#pragma warning(disable: 4512) //'' : assignment operator could not be generated
#pragma warning(disable: 4505) //'' : unreferenced local function has been removed
#pragma warning(disable: 4748) ///GS can not protect parameters and local variables from local buffer overrun because optimizations are disabled in function

#endif

