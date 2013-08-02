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
#include <SFML/Graphics.hpp>
#include "render/RenderThread.h"

void renderingThread(sf::RenderWindow* window)
{
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    // the rendering loop
    while (window->isOpen())
    {
        // draw...
        window->clear();
        window->draw(shape);

        // end the current frame
        window->display();
    }
}
