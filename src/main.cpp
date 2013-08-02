#include "pch.h"
#include "common/global/global.h"
#include <SFML/Graphics.hpp>
#include "render/RenderThread.h"

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
    sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");

    //deactivate window so we can render to it from other thread.
    window.setActive(false);

    //create render thread and start it.
    sf::Thread thread(&renderingThread, &window);
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

