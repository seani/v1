#pragma once

//
//ManagerObject.h and ManagerStatic.h together make up the Module Global Object Management
//system.  See ManagerObject.h for more information.
//

class ManagerStatic;

#include "common\global\global.h"

class ManagerObject;

//
//ManagerStatic is the base class of the type which is responsible for allocating
//the ManagerObject instances, keeping track of ManagerObject reference counts, and
//ultimately for deleting the ManagerObjects.  There is one ManagerStatic object
//instantiated for every module (EXE or DLL).  The ManagerStatic object for a module
//keeps track of how many Item objects from that module are in the ManagerObject,
//and the ManagerStatic object itself is part of a linked list of all ManagerStatic
//objects who's Items are stored in that ManagerObject, forming a "reference count" for
//the ManagerObject instance.
//
//To use ManagerStatic:
//  - Derive a class, "StaticType", from ManagerStaticImpl<> using StaticType
//      and the actual ManagerObject implementation class (derived from ManagerObjectImpl<>) as
//      the template parameters.
//  - Implement the function in StaticType with the signature:
//          - Create(); which returns some ManagerObject-derived object.
//
class ManagerStatic
{
protected:
    ManagerStatic();
    ~ManagerStatic();

    //Add an item from this module to the manager.
    void AddObject(void *object);

    //removes an object from this module from the manager.
    //returns true if there are no more items from this module in the manager.
    bool RemoveObject(void *object);

    //transfers all items from this module to the given manager
    //and makes this module reference that manager.
    void TransferTo(ManagerObject *manager);

    //Wrapper for the Win32 code which will get the address of a transfer function from
    //the loaded DLL, and initiate the transfer of all of that DLL's items to our manager.
    void TransferFromDLL(void *dllHandle, const char *transferFuncName);

public:
    //retreive the one and only active manager object.
    ManagerObject *GetManager();

private:
    //
    ManagerObject *manager;
    int32 itemCount;

    //interface to our derived classes.
    virtual ManagerObject *CreateManager() = 0;

    //removes us from the given manager's references list
    void RemoveModuleReference(ManagerObject *manager);

    //next ManagerStatic module which also references the same manager.
    ManagerStatic *next;
};


//
//ManagerStaticImpl is a typed wrapper around the base ManagerStatic class.
//It defines the static interface used at the very lowest level to 
//insert and remove items from the manager.  It also is the starting point
//when getting the ManagerStatic object for this module and ultimately the
//global manager object itself.
//
template <class StaticType, class ObjectType>
class ManagerStaticImpl : public ManagerStatic
{
public:
    static void AddObject(void *object);
    static void RemoveObject(void *object);
    static void TransferTo(ObjectType *manager);

    //gets the full manager object instance in which this module's items currently reside.
    static ObjectType *GetManager();

private:
    static ManagerStaticImpl<StaticType, ObjectType> *GetInstance();
    static ManagerStaticImpl<StaticType, ObjectType> *instance;

    //true if we've already transfered our items to another manager
    static bool doneTransfer;

    //from ManagerStatic
    ManagerObject *CreateManager();
};


//
//ManagerStaticImpl static members
//

template <class StaticType, class ObjectType>
ManagerStaticImpl<StaticType, ObjectType> *ManagerStaticImpl<StaticType, ObjectType>::instance = NULL;

template <class StaticType, class ObjectType>
bool ManagerStaticImpl<StaticType, ObjectType>::doneTransfer = false;


//
//ManagerStaticImpl template functions
//

template <class StaticType, class ObjectType>
ObjectType *ManagerStaticImpl<StaticType, ObjectType>::GetManager()
{
    //get the static instance
    ManagerStaticImpl<StaticType, ObjectType> *instance = GetInstance();
    IFASSERTNULL(instance == NULL);

    //get the manager, and cast it
    return (ObjectType *)instance->ManagerStatic::GetManager();
}

template <class StaticType, class ObjectType>
ManagerStaticImpl<StaticType, ObjectType> *ManagerStaticImpl<StaticType, ObjectType>::GetInstance()
{
    //check if it exists.
    if (instance == NULL)
    {
        //allocate it
        instance = new ManagerStaticImpl<StaticType, ObjectType>;
        IFASSERTNULL(instance == NULL);
    }

    //return the instance
    return instance;
}

template <class StaticType, class ObjectType>
void ManagerStaticImpl<StaticType, ObjectType>::AddObject(void *object)
{
    IFASSERTRETURN(object == NULL);

    //better not be adding objects after we've already transfered
    IFASSERTRETURN(doneTransfer == true);

    //get the module instance
    ManagerStaticImpl<StaticType, ObjectType> *instance = GetInstance();
    IFASSERTRETURN(instance == NULL);

    //add the object to the instance.
    instance->ManagerStatic::AddObject(object);
}

template <class StaticType, class ObjectType>
void ManagerStaticImpl<StaticType, ObjectType>::RemoveObject(void *object)
{
    IFASSERTRETURN(object == NULL);

    //get the module instance
    ManagerStaticImpl<StaticType, ObjectType> *instance = GetInstance();
    IFASSERTRETURN(instance == NULL);

    //remove object from instance
    instance->ManagerStatic::RemoveObject(object);
}

template <class StaticType, class ObjectType>
void ManagerStaticImpl<StaticType, ObjectType>::TransferTo(ObjectType *manager)
{
    IFASSERTRETURN(manager == NULL);

    //remember that we've transfered our items (so we don't allow further adds)
    doneTransfer = true;

    //get module instance (or create if its not already done)
    ManagerStaticImpl<StaticType, ObjectType> *instance = GetInstance();
    IFASSERTRETURN(instance == NULL);

    //call transfer function in instance.
    instance->ManagerStatic::TransferTo(manager);
}

template <class StaticType, class ObjectType>
ManagerObject *ManagerStaticImpl<StaticType, ObjectType>::CreateManager()
{
    //call our derived class
    return ((StaticType *)this)->Create();
}

