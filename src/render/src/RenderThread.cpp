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
