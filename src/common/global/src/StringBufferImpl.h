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

//We include this file multiple times in order to create different buffer types.
//
//  Basic options set with #define:
//  - StringBufferImpl_ClassName
//      name of the array class we're implementing
//  - StringBufferImpl_CharType
//      the type of character we're using.
//  - StringBufferImpl_Strncpy
//      version of strncpy that we should use.
//  - StringBufferImpl_Sprintf
//      version of sprintf we should use
//  - StringBufferImpl_Strlen
//      version of strlen we should use
//
//This is implemented this way to achieve the highest level of code reuse (not creating the same function multiple times)
//while creating classes which are easy to look at inside the debugger.  If a class heirarchy was used instead, data members
//are buried at the base of the class tree and possibly multiple base classes must be opened in order to view them.
//

template <int32 bufferLen>
class StringBufferImpl_ClassName
{
public:
    explicit StringBufferImpl_ClassName();
    explicit StringBufferImpl_ClassName(StringBufferImpl_CharType const *str);

    //enum which gives the length of the buffer
    enum
    {
        BufferLength = bufferLen
    };

    //sets the string.  pass parameters like printf.
    void Set(StringBufferImpl_CharType const *formatString, ...);

    //sets the string with explicit var args
    void Sprintf(StringBufferImpl_CharType const *formatString, va_list args);

    //copies a given number of characters from a string.
    void Strncpy(StringBufferImpl_CharType const *str, int32 len);

    //appends a string onto what we already contain.
    void Append(StringBufferImpl_CharType const *str);
    void Append(StringBufferImpl_CharType c);

    //returns the name as a null terminated string.
    inline operator StringBufferImpl_CharType const *() const;

    //copies the given null-terminated string.  NULL or empty strings set the buffer to ""
    StringBufferImpl_CharType const *operator=(StringBufferImpl_CharType const *str);

    //returns true if the string has no characters.
    bool Empty();

    //get the string
    StringBufferImpl_CharType const *Str() const;

    #if StringBufferImpl_CharSize == 1
    //convert to utf-8... maybe?
    char const *operator=(wchar const *wcs);
    #endif

protected:
    StringBufferImpl_CharType buffer[bufferLen];
private:
    StringBufferImpl_ClassName(StringBufferImpl_ClassName const &other);
};

template <int32 bufferLen>
StringBufferImpl_ClassName<bufferLen>::StringBufferImpl_ClassName()
{
    buffer[0] = '\0';
}

template <int32 bufferLen>
StringBufferImpl_ClassName<bufferLen>::StringBufferImpl_ClassName(StringBufferImpl_CharType const *str)
{
    //set the string
    *this = str;
}

template <int32 bufferLen>
void StringBufferImpl_ClassName<bufferLen>::Set(StringBufferImpl_CharType const *formatString, ...)
{
    //get our parameter list.
    va_list args;
    va_start(args, formatString);

    //call sprintf
    StringBufferImpl_Sprintf(buffer, bufferLen, formatString, args);

    //put a null at the end.
    buffer[bufferLen - 1] = '\0';
}

template <int32 bufferLen>
void StringBufferImpl_ClassName<bufferLen>::Sprintf(StringBufferImpl_CharType const *formatString, va_list args)
{
    //call sprintf
    StringBufferImpl_Sprintf(buffer, bufferLen, formatString, args);

    //put a null at the end.
    buffer[bufferLen - 1] = '\0';
}

template <int32 bufferLen>
void StringBufferImpl_ClassName<bufferLen>::Strncpy(StringBufferImpl_CharType const *str, int32 len)
{
    //make sure we dont copy more characters than we can
    bound_max(len, bufferLen - 1);

    //copy the characters to our buffer
    ::StringBufferImpl_Strncpy(buffer, str, len);

    //null terminate
    buffer[len] = '\0';
}

template <int32 bufferLen>
void StringBufferImpl_ClassName<bufferLen>::Append(StringBufferImpl_CharType const *str)
{
    //get our current length.
    int32 len = (int32)StringBufferImpl_Strlen(buffer);
    
    //copy characters from the given string.
    ::StringBufferImpl_Strncpy(&buffer[len], str, bufferLen - len - 1);

    //null terminate
    buffer[bufferLen - 1] = '\0';
}

template <int32 bufferLen>
void StringBufferImpl_ClassName<bufferLen>::Append(StringBufferImpl_CharType c)
{
    //get our current length.
    int32 len = (int32)StringBufferImpl_Strlen(buffer);

    //check if we have room
    if (len < bufferLen - 1)
    {
        //add the character
        buffer[len] = c;

        //null terminate
        buffer[len + 1] = 0;
    }
}

template <int32 bufferLen>
StringBufferImpl_ClassName<bufferLen>::operator StringBufferImpl_CharType const *() const
{
    return buffer;
}

template <int32 bufferLen>
StringBufferImpl_CharType const *StringBufferImpl_ClassName<bufferLen>::operator=(StringBufferImpl_CharType const *str)
{
    if (str == NULL || str[0] == '\0')
    {
        buffer[0] = '\0';
        return buffer;
    }

    //copy the string, converting to lower case.
    ::StringBufferImpl_Strncpy(buffer, str, bufferLen - 1);

    //make the last character null.
    buffer[bufferLen - 1] = '\0';

    return buffer;
}

template <int32 bufferLen>
bool StringBufferImpl_ClassName<bufferLen>::Empty()
{
    return buffer[0] == '\0';
}

template <int32 bufferLen>
StringBufferImpl_CharType const *StringBufferImpl_ClassName<bufferLen>::Str() const
{
    //return our buffer
    return buffer;
}

#if StringBufferImpl_CharSize == 1
template <int32 bufferLen>
char const *StringBufferImpl_ClassName<bufferLen>::operator=(wchar const *wcs)
{
    //position we write to
    int32 pos = 0;

    //write characters until we run out of space
    while (*wcs != 0)
    {
        //value of this character
        int32 value = *wcs;

        //check the value
        if (value < 0x7f && pos < bufferLen - 2)
        {
            //this converts to a single character
            buffer[pos] = (char)(uint8)value;
            pos += 1;
        }
        else if (value < 0x7ff && pos < bufferLen - 3)
        {
            //convert to two characters
            buffer[1] = (char)(uint8)(value & 0x3F | 0x80);
            value >>= 6;
            buffer[0] = (char)(uint8)(value & 0x1F | 0xC0);
            pos += 2;
        }
        else if (pos < bufferLen - 4) //else if( value <= 0x0000FFFF )
        {
            //convert to three characters
            buffer[2] = (char)(uint8)(value & 0x3F | 0x80);
            value >>= 6;
            buffer[1] = (char)(uint8)(value & 0x3F | 0x80);
            value >>= 6;
            buffer[0] = (char)(uint8)(value & 0x0F | 0xE0);
            pos += 3;
        }
        //else if( value <= 0x001FFFFF )
        //{
        //    buf[3] = (unsigned char)(value & 0x3F | 0x80);
        //    value>>=6;
        //    buf[2] = (unsigned char)(value & 0x3F | 0x80);
        //    value>>=6;
        //    buf[1] = (unsigned char)(value & 0x3F | 0x80);
        //    value>>=6;
        //    buf[0] = (unsigned char)(value & 0x07 | 0xF0);
        //    return 4;
        //}
        //else if( value <= 0x03FFFFFF )
        //{
        //    buf[4] = (unsigned char)(value & 0x3F | 0x80);
        //    value>>=6;
        //    buf[3] = (unsigned char)(value & 0x3F | 0x80);
        //    value>>=6;
        //    buf[2] = (unsigned char)(value & 0x3F | 0x80);
        //    value>>=6;
        //    buf[1] = (unsigned char)(value & 0x3F | 0x80);
        //    value>>=6;
        //    buf[0] = (unsigned char)(value & 0x03 | 0xF8);
        //    return 5;
        //}
        //else if( value <= 0x7FFFFFFF )
        //{
        //    buf[5] = (unsigned char)(value & 0x3F | 0x80);
        //    value>>=6;
        //    buf[4] = (unsigned char)(value & 0x3F | 0x80);
        //    value>>=6;
        //    buf[3] = (unsigned char)(value & 0x3F | 0x80);
        //    value>>=6;
        //    buf[2] = (unsigned char)(value & 0x3F | 0x80);
        //    value>>=6;
        //    buf[1] = (unsigned char)(value & 0x3F | 0x80);
        //    value>>=6;
        //    buf[0] = (unsigned char)(value & 0x01 | 0xFC);
        //    return 6;
        //}
        else
        {
            //we ran out of space
            break;
        }

        //go to the next character
        wcs++;
    }

    //make sure it's null terminated.
    buffer[pos] = 0;

    //we return ourself
    return &buffer[0];
}
#endif


#undef StringBufferImpl_ClassName
#undef StringBufferImpl_CharType
#undef StringBufferImpl_Strncpy
#undef StringBufferImpl_Sprintf
#undef StringBufferImpl_Strlen
#undef StringBufferImpl_CharSize

