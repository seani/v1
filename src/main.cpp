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

#include "pch.h"
#include "common/global/global.h"
#include <SFML/Graphics.hpp>
#include "render/RenderThread.h"

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "v1 game");

    //deactivate window so we can render to it from other thread.
    window.setActive(false);

    //create render thread and start it.
    sf::Thread thread(RenderingThread, &window);
    thread.launch();

    //keep going forever while we have a window.
    while (window.isOpen())
    {
        sf::Event event;
        while (window.waitEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
    }

    return 0;
}

