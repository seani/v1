#pragma once

#include <string.h>
#include <stdarg.h>
#include "common\global\global.h"

//
//Single byte character string buffer
//

//create single byte character buffer
#define StringBufferImpl_ClassName StringBuffer
#define StringBufferImpl_CharType char
#define StringBufferImpl_CharSize 1
#define StringBufferImpl_Strncpy strncpy_s
#define StringBufferImpl_Sprintf _vsnprintf
#define StringBufferImpl_Strlen strlen
#include "common\global\src\StringBufferImpl.h"

//create double byte character buffer
#include <wchar.h>
#define StringBufferImpl_ClassName StringBufferUnicode
#define StringBufferImpl_CharType wchar
#define StringBufferImpl_CharSize 2
#define StringBufferImpl_Strncpy wcsncpy_s
#define StringBufferImpl_Sprintf _vsnwprintf_s
#define StringBufferImpl_Strlen wcslen
#include "common\global\src\StringBufferImpl.h"


//some buffer types.
typedef StringBuffer<16> buffer16;
typedef StringBuffer<32> buffer32;
typedef StringBuffer<64> buffer64;
typedef StringBuffer<128> buffer128;
typedef StringBuffer<256> buffer256;
typedef StringBuffer<512> buffer512;
typedef StringBuffer<1024> buffer1024;

typedef StringBufferUnicode<16> ubuffer16;
typedef StringBufferUnicode<32> ubuffer32;
typedef StringBufferUnicode<64> ubuffer64;
typedef StringBufferUnicode<128> ubuffer128;
typedef StringBufferUnicode<256> ubuffer256;
typedef StringBufferUnicode<512> ubuffer512;
typedef StringBufferUnicode<1024> ubuffer1024;


//
//Functions usually used when making callback functions for the sorted array classes.
//

template <int32 size> int32 CompareFunc(StringBuffer<size> const &left, StringBuffer<size> const &right)
{
    return CompareFunc((char const *)left, (char const *)right);
}

template <int32 size> int32 CompareFunc(StringBufferUnicode<size> const &left, StringBufferUnicode<size> const &right)
{
    return CompareFunc((wchar const *)left, (wchar const *)right);
}

