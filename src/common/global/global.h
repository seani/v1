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
//Compiler specific stuff
//

#include <math.h>
#include <intrin.h>
#include "common\global\compiler.h"

//
//Integer types
//

typedef __int64 int64;
typedef int int32;
typedef short int16;
typedef char int8;

typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;

typedef wchar_t wchar;

//
//math constants
//

#define PIf (3.1415926535897932384626433832795f)
#define PI2f (6.283185307179586476925286766559f)
#define PIo2f (1.5707963267948966192313216916398f)
#define SR2 (1.4142135623730950488016887242097f)
#define SR2o2 (0.70710678118654752440084436210485f)

//
//delete helpers
//

#define del(var) if (var != null) { delete var; }
#define delc(var) if (var != null) { delete var; var = null; }
#define delca(var) if (var != null) { delete [] var; var = null; }
#define freec(var) if (var != null) { free(var); var = null; }

//
//generic "cleanup" helper
//

#define clean(objPtr, memberFunc) if (objPtr != null) { objPtr->memberFunc(); objPtr = null; } else { }

//
//To do something only once.  Use inside a function body, for example:
//
//  ONCE(spot1) {
//      something()...
//  }
//
#define ONCE(id_token) static int __once_var_##id_token##__ = 0; if (__once_var_##id_token##__++ == 0)

//
//To do something before main.  Use at global scope only.
//
//  INIT(startup_seq1) {
//      something()...
//  }
//
#define INIT(id_token) class __init_class_##id_token##_ {public: __init_class_##id_token##_();}; static __init_class_##id_token##_ __init_inst_##id_token##_; __init_class_##id_token##_::__init_class_##id_token##_()

//
//To do something after main.  Use at global scope only.
//
//  TERM(shutdown_seq1) {
//      something()...
//  }
//
#define TERM(id_token) class __term_class_##id_token##_ {public: ~__term_class_##id_token##_();}; static __term_class_##id_token##_ __term_inst_##id_token##_; __term_class_##id_token##_::~__term_class_##id_token##_()

//
//This macro defines a loop that will run through only once, but gives us a convenient way
//to break out of a block of code, enabling a kind of a goto-but-not-a-goto.
//
//  ONCELOOP(foo) {
//      something()...
//  }
//
#define ONCELOOP(id_token) for (int __once_loop_##id_token##__ = 0; __once_loop_##id_token##__ < 1; __once_loop_##id_token##__++)

//
//BREAK... Not assert, no dialog box; expression still is tested in release mode.
//

#ifdef __CONFIG_DEBUG
    #define IFBREAK(exp) if (exp) { __debugbreak() } else { }
    #define BREAK1() __debugbreak()
#else
    #define IFBREAK(exp)
    #define BREAK1()
#endif

#define IFBREAKRETURN(exp) if (exp) {BREAK1(); return;}
#define IFBREAKNULL(exp) if (exp) {BREAK1(); return null;}
#define IFBREAKFALSE(exp) if (exp) {BREAK1(); return false;}
#define IFBREAKTRUE(exp) if (exp) {BREAK1(); return true;}
#define IFBREAKRETURNVAL(exp, val) if (exp) {BREAK1(); return val;}
#define IFBREAKCONTINUE(exp) if (exp) {BREAK1(); continue;}
#define IFBREAKBREAK(exp) if (exp) {BREAK1(); break;}
#define IFBREAKCONTEXT(exp) if (exp) {BREAK1(); context.CrashMessage(#exp); return false;}
#define IFBREAKCONTEXTMSG(exp, message) if (exp) {BREAK1(); context.CrashMessage(message); return false;}

//same as BREAK, but called ASSERT
#ifdef __CONFIG_FOOBAR
    #define IFASSERTRETURN(exp)
    #define IFASSERTNULL(exp)
    #define IFASSERTFALSE(exp)
    #define IFASSERTTRUE(exp)
    #define IFASSERTRETURNVAL(exp, val)
    #define IFASSERTCONTINUE(exp)
    #define IFASSERTBREAK(exp)
    #define IFASSERT(exp)
    #define ASSERT0()
#else
    #define IFASSERTRETURN(exp) IFBREAKRETURN(exp)
    #define IFASSERTNULL(exp) IFBREAKNULL(exp)
    #define IFASSERTFALSE(exp) IFBREAKFALSE(exp)
    #define IFASSERTTRUE(exp) IFBREAKTRUE(exp)
    #define IFASSERTRETURNVAL(exp, val) IFBREAKRETURNVAL(exp, val)
    #define IFASSERTCONTINUE(exp) IFBREAKCONTINUE(exp)
    #define IFASSERTBREAK(exp) IFBREAKBREAK(exp)
    #define IFASSERT(exp) IFBREAK(exp)
    #define ASSERT0() BREAK1()
#endif

//Shortcuts for "IFASSERT*(string == NULL || string[0] == '\0')
#define IFBADSTRINGRETURN(string) IFASSERTRETURN((string == null) || (string[0] == '\0'))
#define IFBADSTRINGFALSE(string) IFASSERTFALSE((string == null) || (string[0] == '\0'))
#define IFBADSTRINGTRUE(string) IFASSERTTRUE((string == null) || (string[0] == '\0'))
#define IFBADSTRINGNULL(string) IFASSERTNULL((string == null) || (string[0] == '\0'))

//
//Wrappers for making calls to functions that return false to signal an error, and then
//taking some kind of action.
//

#define CALLCHECK(expr) if ((expr) == false) return false;
#define CALLCHECKRETURN(expr) if ((expr) == false) return;

//
//NULL
//

#ifndef NULL
#define NULL 0
#endif

#ifndef null
#define null 0
#endif

//
//bound macros
//

#define bound_min(var, minimumAllowed) if (var < (minimumAllowed)) { var = (minimumAllowed); } else { }
#define bound_max(var, maximumAllowed) if (var > (maximumAllowed)) { var = (maximumAllowed); } else { }
#define bound_var(var, minimumAllowed, maximumAllowed) if (var < (minimumAllowed)) { var = (minimumAllowed); } else if (var > (maximumAllowed)) { var = (maximumAllowed); } else { }
#define Bound3D(varXMin, varXMax, varYMin, varYMax, varZMin, varZMax, valueX, valueY, valueZ) bound_min(varXMax, valueX); bound_min(varYMax, valueY); bound_min(varZMax, valueZ); bound_max(varXMin, valueX); bound_max(varYMin, valueY); bound_max(varZMin, valueZ);

//
//Sets a variable to a value, if its currently null.
//

#define set_var(var, value) if ((var) == null) { (var) = (value); } else { }


//
//shortcut macros for common for loops
//

#define FOR_INT(var, initVal, endCompare, endVal, incOp) for (int var = (initVal); var endCompare (endVal); var incOp)
#define FOR_INT_INC(var, endVal) FOR_INT(var, 0, <, endVal, ++)

//walk an ArrayPointer, which should not change length during the loop.
#define FOR_ARRAY(var, array) for (int var = 0, __##var##End = array.Num(); var < __##var##End; var++)


//
//get number of elements in static array
//
#define ELEMENT_COUNT(staticArray) (sizeof(staticArray) / sizeof(staticArray[0]))

//
//Define a member function and static passthrough
//
#define DEFINE_STATIC_TO_MEMBER_CALLBACK(ReturnType, FuncName, ParamDef) \
    static ReturnType FuncName##Func ParamDef; \
    ReturnType FuncName ParamDef;

#define DECLARE_STATIC_TO_MEMBER_CALLBACK(ReturnType, ClassName, FuncName, ParamDef, Parameters, GlobalInstance) \
    ReturnType ClassName::FuncName##Func ParamDef { /* call to global instance */ return GlobalInstance.FuncName Parameters; } \
    ReturnType ClassName::FuncName ParamDef

//
//For windows.h challenged cpp files
//
#ifndef _WINDEF_
    #define HWND uint32
    #define HDC uint32
    #define HBITMAP uint32
    #define HANDLE uint32
    #define DWORD unsigned long
    #define WINAPI __stdcall
    #define HINSTANCE uint32
    #define LPWSTR wchar *
#endif

//everyone uses the context code, or at least they should
#include "common\global\ArrayPointer.h"
#include "common\global\StringBuffer.h"
#include "common\global\Context.h"
