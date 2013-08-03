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
#include <SFML\Graphics.hpp>
#include "render\RenderThread.h"

//text drawing interface
#include "sim\IScreenText.h"
static IScreenText *screenText;
InterfaceReference(screenText, IScreenText, default);

//time interface
#include "sim\ITime.h"
static ITime *timer;
InterfaceReference(timer, ITime, default);

//frame rate tracking interface
#include "sim\IFrameRate.h"
static IFrameRate *frameRate;
InterfaceReference(frameRate, IFrameRate, default);


void RenderingThread(sf::RenderWindow* window)
{
    CONTEXT_ROOT();

    //initialize screen text module
    screenText->Startup(context);

    //load a texture.
    sf::Texture grassTexture;
    IFBREAKCONTEXTRETURN(grassTexture.loadFromFile("art/textures/grass.png") == false);

    //dimensions of grass texture
    sf::Vector2u grassSize = grassTexture.getSize();

    //make a sprite with our texture.
    sf::Sprite sprite;
    sprite.setTexture(grassTexture);

    //initialize time module
    timer->Startup(context);

    // the rendering loop
    while (window->isOpen())
    {
        //register the start of this frame.
        timer->UpdateTime();

        //time spent doing last frame.
        float frameTime = timer->FrameTime();

        //do computation for our frame rate.
        frameRate->FrameStatistics(frameTime, context);

        //clear screen.
        //window->clear();

        //now many rows and columns of sprites to fill our screen.
        int numRows = numRows = (window->getSize().y + grassSize.y - 1) / grassSize.y;
        int numColumns = (window->getSize().x + grassSize.x - 1) / grassSize.x;

        //fill screen with grass, one row at a time.
        for (int row = 0; row < numRows; row++)
        {
            //go across each row.
            for (int column = 0; column < numColumns; column++)
            {
                //move sprite to this position
                sprite.setPosition(float(column * grassSize.x), float(row * grassSize.y));

                //draw the grass
                window->draw(sprite);
            }
        }

        //render text overlays
        screenText->RenderText(window, context);

        //show the stuff we drew to the window.
        if (window->isOpen() == true) window->display();
    }
}
