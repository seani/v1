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
//ManagerObject.h and ManagerStatic.h together make up the Module Global Object Management
//system.  This system is for the implementation of a system with the following components:
//
//  - A set of modules.  Main module is the EXE.  Additional modules are DLLs.
//  - Each module has a collection of static global objects, called items.
//  - Each module, as it is loaded creates a manager object, and all
//      global items are added to the single manager object (before main is called).
//  - When one module (EXE or DLL) loads a DLL, the items in the DLL's manager are added to the
//      manager object in the loading module and the DLL's manager object is destroyed.
//  - When a DLL is unloaded and its global items are destroyed, they are 
//      safely removed from whatever manager object currently stores them.
//  - When all global items (in an EXE or DLL) are removed from any manager (on shutdown)
//      the manager object is deleted.
//
//There are two main components to the system, the ManagerObject and the ManagerStatic.
//ManagerObject holds the items.  In general, there will be only one of these objects in 
//the entire process.  The only time there will be more than one is immediately after
//a DLL is loaded.  As soon as DLL initialization is complete, the items in the loaded
//DLL's ManagerObject will be transfered to the ManagerObject of the module which loaded
//the DLL, and the DLL's ManagerObject is destroyed.
//
//ManagerStatic is responsible for creating and destroying the ManagerObjects.  See
//ManagerStatic.h for more information.
//

class ManagerObject;

#include "common\global\global.h"

class ManagerStatic;

//
//ManagerObject is the base object type for the actual manager which stores and
//manipulates the Items collected from the current module and other modules.
//It serves as an interface for the generic ManagerStatic functions to to call into the 
//implementation specific Manager classes.
//
//An object of this type is allocated and managed by a ManagerStatic implementation.
//
//To use ManagerObject:
//  - Derive a class "ObjectType" from ManagerObjectImpl<ObjectType>, using the derived class
//      name as the template parameter.
//  - Define the class "ObjectType"::Item in the derived type.
//  - Implement functions in "ObjectType" with the signatures:
//          - Add(Item *item);
//          - Remove(Item *item);
//          - Transfer(ObjectType *manager);
//

class ManagerObject
{
public:
    inline ManagerObject();
    virtual ~ManagerObject() {}

private:
    //this whole base interface is for the sole use of ManagerStatic.
    friend ManagerStatic;

    //Adds an object to the manager.
    virtual void AddObject(void *object) = 0;

    //removes an object from the manager.
    virtual void RemoveObject(void *object) = 0;

    //transfer our items to the given manager
    virtual void TransferTo(ManagerObject *manager) = 0;

    //the linked list of modules which reference this object
    ManagerStatic *references;
};


//
//Wrapper to translate calls from the void * interface of ManagerObject to
//the type specific interface of ObjectType.
//
template <class ObjectType>
class ManagerObjectImpl : public ManagerObject
{
private:
    //from ManagerObject
    void AddObject(void *object);
    void RemoveObject(void *object);
    void TransferTo(ManagerObject *manager);
};


//
//ManagerObject inline functions
//

ManagerObject::ManagerObject()
{
    //no modules references us.
    references = NULL;
}


//
//ManagerObjectImpl template functions
//

template <class ObjectType>
void ManagerObjectImpl<ObjectType>::AddObject(void *object)
{
    //cast the pointer, and call our derived class.
    ((ObjectType *)this)->Add((ObjectType::Item *)object);
}

template <class ObjectType>
void ManagerObjectImpl<ObjectType>::RemoveObject(void *object)
{
    //cast the pointer, and call our derived class
    ((ObjectType *)this)->Remove((ObjectType::Item *)object);
}

template <class ObjectType>
void ManagerObjectImpl<ObjectType>::TransferTo(ManagerObject *manager)
{
    //cast the pointer, and call our derived class
    ((ObjectType *)this)->Transfer((ObjectType *)manager);
}


