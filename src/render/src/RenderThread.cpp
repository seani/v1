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

void RenderingThread(sf::RenderWindow* window)
{
    CONTEXT_ROOT();

    //load a texture.
    sf::Texture grassTexture;
    IFBREAKCONTEXTRETURN(grassTexture.loadFromFile("art/textures/grass.png") == false);

    //dimensions of grass texture
    sf::Vector2u grassSize = grassTexture.getSize();

    //make a sprite with our texture.
    sf::Sprite sprite;
    sprite.setTexture(grassTexture);

    // the rendering loop
    while (window->isOpen())
    {
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

        //show the stuff we drew to the window.
        window->display();
    }
}
