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

//for the database objects.  
#include "common\idb\InterfaceDatabase.h"

//
//The base of all interface classes managed by the database
//

class IBase
{
public:
    //function defined by the implementation class
    virtual const char *__implName() = 0;
};

//
//Interface implementation macro.  This macro is placed at the top
//of an interface implementation class in a public block.  The macro
//gives bodies to some IBase inherited functions.
//

#define InterfaceImplementation(implClassName) \
    virtual const char *__implName() {return #implClassName;}


//
//Adds an interface implementation instantiation to the database.
//It creates a static object of type InterfaceDatabase::Implementation.
//The first parameter is the actual statically defined object of the implementation class.
//The second and third parameters are the name of the interface and the instance name 
//that the object implements.
//

#define InterfaceRegister(instObj, ifaceName, instName) \
    static InterfaceDatabase::Implementation __implVar_##ifaceName##_##instName##_(static_cast<ifaceName &>(instObj), instObj.__implName(), #ifaceName "." #instName);

#define InterfaceInNamespaceRegister(instObj, namespaceName, ifaceName, instName) \
    static InterfaceDatabase::Implementation __implVar_##namespaceName##_##ifaceName##_##instName##_(static_cast<ifaceName &>(instObj), instObj.__implName(), #namespaceName "::" #ifaceName "." #instName);

//
//Instantiate and register an implementation class
//

#define InterfaceCreate(implClass, implVar, ifaceName, instName) \
    static implClass implVar; \
    InterfaceRegister(implVar, ifaceName, instName);

#define InterfaceInNamespaceCreate(implClass, implVar, namespaceName, ifaceName, instName) \
    static implClass implVar; \
    InterfaceInNamespaceRegister(implVar, namespaceName, ifaceName, instName);


//
//Adds an interface reference to the database.  It creates a static object
//of type InterfaceDatabase::reference.  The first parameter
//is a statically defined pointer to the desired interface class.  
//The second and third parameters are the name of the interface and the instance name
//of the desired interface.
//

#define InterfaceReference(refVar, ifaceName, instName) \
    static InterfaceDatabase::Reference __refVar_##ifaceName##_##instName##_((IBase **)&refVar, #ifaceName "." #instName);

#define InterfaceInNamespaceReference(refVar, namespaceName, ifaceName, instName) \
    static InterfaceDatabase::Reference __refVar_##namespaceName##_##ifaceName##_##instName##_((IBase **)&refVar, #namespaceName "::" #ifaceName "." #instName);

