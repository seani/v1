#include "common\global\ManagerStatic.h"
#include "common\global\ManagerObject.h"

//
//ManagerStatic functions
//

ManagerStatic::ManagerStatic()
{
    //we have no manager yet
    manager = NULL;

    //no items in the manager from this module
    itemCount = 0;

    //we're not in a list
    next = NULL;
}

ManagerStatic::~ManagerStatic()
{

}

void ManagerStatic::AddObject(void *object)
{
    IFASSERTRETURN(object == NULL);

    //get the manager
    ManagerObject *manager = GetManager();
    IFASSERTRETURN(manager == NULL);

    //add the object to the manager
    manager->AddObject(object);

    //we have increased the count of items in the manager.
    itemCount++;
}

bool ManagerStatic::RemoveObject(void *object)
{
    IFASSERTFALSE(object == NULL);

    //our manager better still be around (and already created)
    IFASSERTFALSE(manager == NULL);

    //remove the object
    manager->RemoveObject(object);

    //we have decreased the count of items in the manager
    itemCount--;

    //check if all of this module's items have been removed
    if (itemCount > 0)
    {
        //still items from this module in the manager.
        return false;
    }

    //all items have been removed.

    //remove this module from the list of references in our manager
    RemoveModuleReference(manager);

    //check if there are no more modules referencing the manager
    if (manager->references == NULL)
    {
        //delete the manager object
        delc(manager);
    }

    //this module has no more items in the manager.
    return true;
}

void ManagerStatic::TransferTo(ManagerObject *newManager)
{
    //check if we have created a manager.
    if (manager == NULL)
    {
        //we never created a manager, which means we have no items from
        //this module that need to be added to the manager.
        //However, we do need to keep a reference to the new manager,
        //so that this module can access that manager through the static interface.
        manager = newManager;
        return;
    }

    //get our current manager (or create it if we dont have one yet)
    ManagerObject *currentManager = GetManager();
    IFASSERTRETURN(currentManager == NULL);

    //transfer all objects to the new manager
    currentManager->TransferTo(newManager);

    //all modules which referenced the old manager now reference the new manager.
    for (ManagerStatic *module = currentManager->references; module != NULL;)
    {
        //this module now references the new manager
        module->manager = newManager;

        //get the next module
        ManagerStatic *next = module->next;

        //add this module to the list of references of the new manager
        module->next = newManager->references;
        newManager->references = module;

        //go to next module in the list of the old manager's references
        module = next;
    }

    //nobody references the old manager any more, delete it
    delc(currentManager);
}

ManagerObject *ManagerStatic::GetManager()
{
    //check if we have it
    if (manager == NULL)
    {
        //don't have the manager yet.
        //create one
        manager = CreateManager();
        IFASSERTNULL(manager == NULL);

        //add ourself to the list of modules using this manager
        next = manager->references;
        manager->references = this;
    }

    //return the manager we've created.
    return manager;
}

void ManagerStatic::RemoveModuleReference(ManagerObject *manager)
{
    IFASSERTRETURN(manager == NULL);

    //there better be at least 1 reference in the manager's list
    IFASSERTRETURN(manager->references == NULL);

    //check if we're the first reference
    if (this == manager->references)
    {
        //we are the first.  make the list start at the next one
        manager->references = this->next;

        //done
        return;
    }

    //we're not the first, walk the list till we find the one that
    //points to us as next
    for (ManagerStatic *module = manager->references; module->next != NULL; module = module->next)
    {
        //check if we're next in the list after this module
        if (module->next == this)
        {
            //make the list skip up
            module->next = this->next;

            //done
            return;
        }
    }

    //we weren't in the list...
    BREAK1();
}

