#include "pch.h"
#include <SFML\Graphics.hpp>
#include "sim\IScreenText.h"


//our implementation of IScreenText
class ScreenTextImpl : public IScreenText
{
public:
    InterfaceImplementation(ScreenTextImpl);

    ScreenTextImpl();

    //from IScreenText
    bool Startup(Context &caller);
    bool RenderText(sf::RenderWindow *window, Context &caller);
    bool PrintLineTopLeft(wchar const *text, Context &caller);

    //text strings we show on top left
    ArrayPointerOwner<ubuffer256> topLeft;

    //spare text strings
    ArrayPointerOwner<ubuffer256> spares;

    //our font
    sf::Font *font;
};

//instantiate and register our implementation
InterfaceCreate(ScreenTextImpl, screenTextImpl, IScreenText, default);

//
//ScreenTextImpl functions
//

ScreenTextImpl::ScreenTextImpl()
{
    font = null;
}

bool ScreenTextImpl::Startup(Context &caller)
{
    CONTEXT_CALLED();

    //create font object
    font = new sf::Font();
    IFBREAKCONTEXT(font == null);

    //try to load our font
    IFBREAKCONTEXT(font->loadFromFile("art/fonts/FreeMono.ttf") == false);

    //success
    return true;
}

bool ScreenTextImpl::RenderText(sf::RenderWindow *window, Context &caller)
{
    CONTEXT_CALLED();
    IFBREAKFALSE(window == null);
    IFBREAKFALSE(font == null);

    //check if we have text for the top left
    if (topLeft.Num() > 0)
    {
        //top of the next line we draw.
        float y = 5.0f;

        //get strings until there are no more
        for (int32 strIndex = 0; strIndex < topLeft.Num(); strIndex++)
        {
            //get first string
            ubuffer256 *str = topLeft.Get(strIndex);

            //text object we draw with
            sf::Text text;

            //set the font
            text.setFont(*font);

            //set the string to display
            text.setString((const wchar *)*str);

            //set text size
            text.setCharacterSize(16);

            //first we draw black version as a shadow
            text.setColor(sf::Color::Black);

            //draw shadow text
            text.setPosition(6.0f, y);
            window->draw(text);
            text.setPosition(6.0f, y + 1.0f);
            window->draw(text);
            text.setPosition(5.0f, y + 1.0f);
            window->draw(text);

            //make it white text
            text.setColor(sf::Color::White);

            //set position
            text.setPosition(5.0f, y);

            //draw the text to the window
            window->draw(text);

            //next string goes down a line
            y += 12 + 0;
        }

        //remove all our strings back to the spare pool
        while (topLeft.Num() > 0)
        {
            //remove one
            ubuffer256 *str = topLeft.Remove(0);

            //put it in spare pool
            spares.Add(str);
        }
    }

    //success
    return true;
}

bool ScreenTextImpl::PrintLineTopLeft(wchar const *text, Context &caller)
{
    CONTEXT_CALLED();
    IFBREAKCONTEXT(text == null);

    //string we will store the text in
    ubuffer256 *str = null;

    //check if we have a string already allocated
    if (spares.Num() > 0)
    {
        //get one out
        str = spares.Remove(0);
    }
    else
    {
        //create a new one
        str = new ubuffer256();
    }

    //add the string to our list
    topLeft.Add(str);

    //set the text
    *str = text;

    //success
    return true;
}

