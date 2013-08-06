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

#include "common\idb\src\InterfaceDatabaseImpl.h"


//
//InterfaceDatabaseStatic functions
//

ManagerObject *InterfaceDatabaseStatic::Create()
{
    //allocate our implementation type
    InterfaceDatabaseObject *database = new InterfaceDatabaseObject();
    IFASSERTNULL(database == NULL);

    //return the crated database
    return database;
}


//
//InterfaceDatabaseObject functions
//

void InterfaceDatabaseObject::Add(Item *object)
{
    //cast to both types
    Implementation *implementation = object->ToImplementation();
    Reference *reference = object->ToReference();

    //check if its is an implementation
    if (implementation != NULL)
    {
        //add it as an implementation
        Add(implementation);
    }
    //check if it is a reference
    else if (reference != NULL)
    {
        //add it as a reference
        Add(reference);
    }
    else
    {
        //messed up object
        BREAK1();
    }
}

void InterfaceDatabaseObject::Remove(Item *object)
{
    //check if its is an implementation
    if (object->ToImplementation() != NULL)
    {
        //remove it as an implementation
        Remove(object->ToImplementation());
    }
    //check if it is a reference
    else if (object->ToReference() != NULL)
    {
        //remove it as a reference
        Remove(object->ToReference());
    }
    else
    {
        //messed up object
        BREAK1();
    }
}

void InterfaceDatabaseObject::Add(InterfaceDatabase::Implementation *impl)
{
    //get the name of the implemented interface
    const char *ifaceName = impl->GetInterfaceName();
    IFBREAKRETURN(ifaceName == NULL);

    //find the UniqueInterface instance for this interface
    UniqueInterface *iface = GetIface(ifaceName);
    IFBREAKRETURN(iface == NULL);

    //add the implementation to the iface
    iface->Add(impl);
}

void InterfaceDatabaseObject::Remove(InterfaceDatabase::Implementation *impl)
{
    //get the name of the implemented interface
    const char *ifaceName = impl->GetInterfaceName();
    IFBREAKRETURN(ifaceName == NULL);

    //find the UniqueInterface instance for this interface
    UniqueInterface *iface = GetIface(ifaceName);
    IFBREAKRETURN(iface == NULL);

    //remove the implementation
    iface->Remove(impl);
}

void InterfaceDatabaseObject::Add(InterfaceDatabase::Reference *ref)
{
    //get the name of the requested interface
    const char *ifaceName = ref->GetInterfaceName();
    IFBREAKRETURN(ifaceName == NULL);

    //find the UniqueInterface instance for this interface
    UniqueInterface *iface = GetIface(ifaceName);
    IFBREAKRETURN(iface == NULL);

    //add the implementation to the iface
    iface->Add(ref);
}

void InterfaceDatabaseObject::Remove(InterfaceDatabase::Reference *ref)
{
    //get the name of the referenced interface
    const char *ifaceName = ref->GetInterfaceName();
    IFBREAKRETURN(ifaceName == NULL);

    //find the UniqueInterface instance for this interface
    UniqueInterface *iface = GetIface(ifaceName);
    IFBREAKRETURN(iface == NULL);

    //remove the reference
    iface->Remove(ref);
}

InterfaceDatabaseObject::UniqueInterface *InterfaceDatabaseObject::GetIface(char const *name)
{
    //check if we already have one
    UniqueInterface *iface = database.Find(name, FindIFace);
    if (iface != NULL)
    {
        return iface;
    }

    //we need to create a new database element
    iface = new UniqueInterface(name);

    //insert the iface into the database
    database.Add(iface, CompareIFace);

    //return it.
    return iface;
}

void InterfaceDatabaseObject::Transfer(InterfaceDatabase *manager)
{
    IFASSERTRETURN(manager == NULL);

    //cast manager to database
    InterfaceDatabaseObject *other = (InterfaceDatabaseObject *)manager;

    //go through all of our interfaces
    while (database.Num() > 0)
    {
        //remove an interface
        UniqueInterface *iface = database.Remove(database.Num() - 1);
        IFBREAKCONTINUE(iface == NULL);

        //transfer all the objects to the other database.
        iface->TransferTo(other);

        //delete the iface
        delete iface;
    }
}

//
//InterfaceDatabaseObject::UniqueInterface functions
//

InterfaceDatabaseObject::UniqueInterface::UniqueInterface(char const *name)
{
    //save the name
    interfaceName = name;

    //we have no current implementation.
    curImpl = NULL;
}

const char *InterfaceDatabaseObject::UniqueInterface::GetName() const
{
    return interfaceName;
}

void InterfaceDatabaseObject::UniqueInterface::Add(InterfaceDatabase::Implementation *impl)
{
    IFBREAKRETURN(impl == NULL);

    //insert the implementation into our list
    implementations.Add(impl);

    //check if we already have an implementation active.
    if (curImpl == NULL)
    {
        //this is the first impl we have so far.
        UseImplementation(impl);
    }
}

void InterfaceDatabaseObject::UniqueInterface::Remove(InterfaceDatabase::Implementation *impl)
{
    IFBREAKRETURN(impl == NULL);

    //check if this is the one currently selected
    if (impl == curImpl)
    {
        //disconnect all references
        DisconnectReferences();

        //we have no current implementation
        curImpl = NULL;
    }

    //remove it from the list
    implementations.Remove(impl);

    //check if there are any others left
    if (implementations.Num() > 0)
    {
        //get another implementation
        Implementation *new_impl = implementations.Get(0);

        //check if there is one.
        if (new_impl != NULL)
        {
            //use this implemenation
            UseImplementation(new_impl);
        }
    }
}

void InterfaceDatabaseObject::UniqueInterface::Add(InterfaceDatabase::Reference *ref)
{
    IFBREAKRETURN(ref == NULL);

    //insert the reference into our list
    references.Add(ref);

    //check if we have an active implementation.
    if (curImpl != NULL)
    {
        //this is the first impl we have so far.
        ref->UseImplementation(curImpl->GetInstance());
    }
}

void InterfaceDatabaseObject::UniqueInterface::Remove(InterfaceDatabase::Reference *ref)
{
    IFBREAKRETURN(ref == NULL);

    //remove it
    references.Remove(ref);

    //disconnect it
    ref->UseImplementation(NULL);
}

void InterfaceDatabaseObject::UniqueInterface::UseImplementation(InterfaceDatabase::Implementation *impl)
{
    IFBREAKRETURN(impl == NULL);

    //do nothing if we are already using this implementation
    if (impl == curImpl) 
    {
        return;
    }

    //set this as our current implementation
    curImpl = impl;

    //make all the references use this implementation.
    int32 num_refs = references.Num();
    for (int32 i = 0; i < num_refs; i++)
    {
        //get the reference
        Reference *ref = references.Get(i);
        IFBREAKCONTINUE(ref == NULL);

        //make the reference use this implementation
        ref->UseImplementation(impl->GetInstance());
    }
}

void InterfaceDatabaseObject::UniqueInterface::TransferTo(InterfaceDatabaseObject *db)
{
    IFBREAKRETURN(db == NULL);

    //first disconnect all our references
    DisconnectReferences();

    //remove all our reference objects and add them to the other database.
    while (references.Num() > 0)
    {
        //remove one
        Reference *ref = references.Remove(0L);

        //add it to the other database
        db->Add(ref);
    }

    //remove all our implementations and add them to the other database
    while (implementations.Num() > 0)
    {
        //remove one
        Implementation *impl = implementations.Remove(0L);

        //add it to the other database
        db->Add(impl);
    }

    //done
}

void InterfaceDatabaseObject::UniqueInterface::DisconnectReferences()
{
    //get the number of references
    int32 num_refs = references.Num();

    //go through all the references
    for (int32 i = 0; i < num_refs; i++)
    {
        //get the reference
        Reference *ref = references.Get(i);

        //disconnect it
        ref->UseImplementation(NULL);
    }
}

const char *InterfaceDatabaseObject::UniqueInterface::GetInterfaceName()
{
    //return our name
    return interfaceName;
}

IBase *InterfaceDatabaseObject::UniqueInterface::GetActiveInstance()
{
    //check if there's a current instance
    if (curImpl == NULL)
    {
        //no instance
        return NULL;
    }

    //get the address of the implementation
    return curImpl->GetInstance();
}


//
//InterfaceDatabase::Item functions
//

InterfaceDatabase::Implementation *InterfaceDatabase::Item::ToImplementation()
{
    //by default, we arent an implementation
    return NULL;
}

InterfaceDatabase::Reference *InterfaceDatabase::Item::ToReference()
{
    //by default, we aren't a reference
    return NULL;
}


//
//InterfaceDatabase::Implementation functions
//

InterfaceDatabase::Implementation::Implementation(IBase &impl, const char *implClassName, const char *interfaceName)
{
    //save a pointer to the implementation
    this->impl = &impl;

    //save the name of the implemented interface
    implementedInterfaceName = interfaceName;

    //save the name of the implementation class
    implementationName = implClassName;

    //add ourself to the global manager
    InterfaceDatabaseStatic::AddObject(this);
}

InterfaceDatabase::Implementation::~Implementation()
{
    //remove ourselves from the global manager
    InterfaceDatabaseStatic::RemoveObject(this);
}

InterfaceDatabase::Implementation *InterfaceDatabase::Implementation::ToImplementation()
{
    //we're an implementation
    return this;
}

const char *InterfaceDatabase::Implementation::GetInterfaceName()
{
    return implementedInterfaceName;
}

const char *InterfaceDatabase::Implementation::GetImplemenationName()
{
    return implementationName;
}

IBase *InterfaceDatabase::Implementation::GetInstance()
{
    return impl;
}


//
//InterfaceDatabase::Reference functions
//

InterfaceDatabase::Reference::Reference(IBase **ifaceRef, const char *interfaceName)
{
    //save the passed in parameters.
    this->ifaceRef = ifaceRef;
    this->interfaceName = interfaceName;

    //initialize the pointer
    *ifaceRef = NULL;

    //add ourselves to the global manager
    InterfaceDatabaseStatic::AddObject(this);

    //this better never fire.
    IFBREAKRETURN(interfaceName == NULL || ifaceRef == NULL);
}

InterfaceDatabase::Reference::~Reference()
{
    //set the pointer to NULL
    if (ifaceRef != NULL)
    {
        *ifaceRef = NULL;
    }

    //remove ourselves from the global manager
    InterfaceDatabaseStatic::RemoveObject(this);
}

InterfaceDatabase::Reference *InterfaceDatabase::Reference::ToReference()
{
    //we are a reference object
    return this;
}

const char *InterfaceDatabase::Reference::GetInterfaceName()
{
    return interfaceName;
}

void InterfaceDatabase::Reference::UseImplementation(IBase *impl)
{
    //change the pointer
    *ifaceRef = impl;
}

