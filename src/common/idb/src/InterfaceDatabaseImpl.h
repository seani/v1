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
//Actual implementation of interface database, which stores the the
//InterfaceManager item objects.
//

class InterfaceDatabaseStatic;
class InterfaceDatabaseObject;

#include "common\idb\InterfaceDatabase.h"
#include "common\global\ManagerStatic.h"

//
//ManagerStatic implementation.
//
class InterfaceDatabaseStatic : public ManagerStaticImpl<InterfaceDatabaseStatic, InterfaceDatabaseObject>
{
public:
    //from ModuleStatic
    ManagerObject *Create();
};

//
//Full ManagerObject implementation
//
class InterfaceDatabaseObject : public InterfaceDatabase
{
public:

private:
    //from InterfaceManager.
    void Add(Item *object);
    void Remove(Item *object);
    void Transfer(InterfaceDatabase *manager);

    //specialized versions of Add and Remove
    void Add(Implementation *impl);
    void Add(Reference *ref);
    void Remove(Implementation *impl);
    void Remove(Reference *ref);

    //each unique interface name has a section in the database
    //A unique interface is a combination interface and instance name.
    class UniqueInterface
    {
    public:
        UniqueInterface(char const *name);

        //returns the name of the interface we manage.
        const char *GetName() const;

        //adds/removes an interface implementation
        void Add(Implementation *impl);
        void Remove(Implementation *impl);

        //adds/removes an interface reference
        void Add(Reference *ref);
        void Remove(Reference *ref);

        //transfers all our objects to the given database
        void TransferTo(InterfaceDatabaseObject *db);

        //gets our interface name
        const char *GetInterfaceName();

        //gets instace which is currently active
        IBase *GetActiveInstance();

    private:
        //the name of the interface
        buffer128 interfaceName;

        //the implementations of this interface that we currently know about
        ArrayPointer<Implementation> implementations;

        //the addresses of references to this interface
        ArrayPointer<Reference> references;

        //the implementation that is currently being used
        Implementation *curImpl;

    private:
        //activates the given implementation, all references will point to it.
        void UseImplementation(Implementation *impl);

        //disconnects all references
        void DisconnectReferences();
    };

    //The main data in the database, the collection of UniqueInterfaces
    ArraySortedOwner<UniqueInterface> database;

    //finds or creates a UniqueInterface for the given interface name.
    UniqueInterface *GetIface(char const *name);

    //compare and find functions
    SortedArrayFunctions(CompareIFace, FindIFace, UniqueInterface, char const *, GetName());
};

