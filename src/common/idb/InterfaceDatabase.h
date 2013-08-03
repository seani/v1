#pragma once

//
//ManagerObject partial implemenation for the Interface Database system.
//
//The main purpose of this class is to define the Item type and
//its derived types.  These Items are the objects which are globally instantiated
//and which insert themselves into the manager.
//This class also acts as the abstract interface which our base class uses
//to access the full implementation which derives from this class.
//

#include "common\global\ManagerObject.h"

class IBase;

class InterfaceDatabase : public ManagerObjectImpl<InterfaceDatabase>
{
public:
    //the items we add to the interface database.
    class Implementation;
    class Reference;

    //our managed global items
    class Item
    {
    public:
        //cast ourself to one of the derived types
        virtual Implementation *ToImplementation();
        virtual Reference *ToReference();
    };

    //interface database Implementation object.
    class Implementation : public Item
    {
    public:
        Implementation(IBase &impl, const char *implClassName, const char *interfaceName);
        ~Implementation();

        //from Item
        Implementation *ToImplementation();
    
        //returns name of the implemented interface
        const char *GetInterfaceName();

        //returns the name of the implementation class
        const char *GetImplemenationName();

        //returns the address of the implementation instantiation
        IBase *GetInstance();

    private:
        //the actual implementation of an interface
        IBase *impl;

        //the name of the implemenation class
        const char *implementationName;

        //the name of the interface that is implemented.
        const char *implementedInterfaceName;
    };

    //interface database Reference object
    class Reference : public Item
    {
    public:
        Reference(IBase **ifaceRef, const char *interfaceName);
        ~Reference();

        //from Item
        Reference *ToReference();

        //returns the name of the interface we want
        const char *GetInterfaceName();

        //switches the implementation we point at.
        void UseImplementation(IBase *impl);

    private:
        //reference to the interface pointer
        IBase **ifaceRef;

        //the name of the interface
        const char *interfaceName;
    };

    //called from templated base class functions.
    virtual void Add(Item *object) = 0;
    virtual void Remove(Item *object) = 0;
    virtual void Transfer(InterfaceDatabase *manager) = 0;
};

