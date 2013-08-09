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

//We include this file multiple times in order to create different array types.
//
//  Basic options set with #define:
//  - ArrayPointerImpl_ClassName
//      name of the array class we're implementing
//  - ArrayPointerImpl_Owner
//      present if we want our implementation to own the objects and be responsible for deleting them
//      Changes destructor, Empty, Reset to O(n)
//  - ArrayPointerImpl_Queue
//      Present if we want our implementation to preseve the order of items when items are removed.
//      Changes Remove to be O(n)
//      Adds O(1) Pop to remove the oldest item.
//  - ArrayPointerImpl_Sorted
//      Adds functions to support O(log(n)) searching for items.
//      Implies ArrayPointerImpl_Queue
//
//This is implemented this way to achieve the highest level of code reuse (not creating the same function multiple times)
//while creating classes which are easy to look at inside the debugger.  If a class heirarchy was used instead, data members
//are buried at the base of the class tree and possibly multiple base classes must be opened in order to view them.
//


template <class C>
class ArrayPointerImpl_ClassName
{
private:
    ArrayPointerImpl_ClassName(ArrayPointerImpl_ClassName const &other);
public:
    ArrayPointerImpl_ClassName();
    ~ArrayPointerImpl_ClassName();

    #ifdef ArrayPointerImpl_Sorted
    //
    //Sorted array function pointer types.
    //

    //compare function type.  The parameters are double pointers so that we can 
    //use this function directly in the CRT qsort routine.
    typedef int32 (*CompareFunc)(C const **item1, C const **item2);
    
    //similar to the CompareFunc, but passes a void identifier instead of the first item.
    template <class ID>
    class FindFunc { public:
        typedef int32 (*Func)(ID const id, C const *item);
    };
    #endif

    //
    //member access functions
    //
public:
    int32 Num() const;
    C *Get(int32 index);
    inline C const *Get(int32 index) const;

    //get all of our elements
    C const *const *Array() const;

    //remove all our items and add all items from the given array, removing them from it
    void TakeFrom(ArrayPointerImpl_ClassName &other);

    //
    //add a single item to the array
    //
public:
    //adds an item to the array.
    #if defined(ArrayPointerImpl_Sorted)
    void Add(C *item, CompareFunc compareFunc);
private:
    bool Insert(C *item, int32 index);
    #endif
    void Add(C *item);

    //
    //Remove an item from the array.  The object's lifetime must be then managed by the caller.
    //
public:
    //removes an item given its index or value.
    C *Remove(int32 index);
    C *Remove(C *item);
    #if defined(ArrayPointerImpl_Sorted)
    template <class ID>
    C *Remove(ID const id, typename FindFunc<ID>::Func findFunc);
    #endif

    //
    //Find an item in the array.
    //
public:
    #if defined(ArrayPointerImpl_Sorted)
    //finds the item that matches the given identifier, given a find function
    template <class ID>
    C *Find(ID const id, typename FindFunc<ID>::Func findFunc);
    //finds an item that matches the given item according to the given Compare function
    //returns NULL if no item matches.
    C *Find(C const *item, CompareFunc compareFunc);
    #endif

    //
    //Functions to clear the array.
    //
    //remove all elements from our array and freeing storage space.
    void Reset();

    //remove all elements from our array without decreasing max capacity.
    void Empty();

private:
    //the number of elements in our array.
    int32 num;

    //the length of the pointer array.
    int32 max;

    #if defined(ArrayPointerImpl_Queue)
    //start index of first, second, and third blocks
    int32 start[3];

    //number of items in first, second, and third blocks
    int32 count[3];
    #endif

    #ifdef __CONFIG_DEBUG
    //In DEBUG, we use this to easier look at the
    //first item pointers in the array.
    typedef C *sixteenOfEm[16];
    union
    {
        //normal array of object pointers.
        C **items;

        //pointer to array of 16 pointers.
        sixteenOfEm *items16;
    };
    #else
    //our array of object pointers.
    C **items;
    #endif

    //doubles the size of the array of pointers
    bool Lengthen();
};

//
//ArrayPointerImpl_ClassName template functions
//

template <class C>
ArrayPointerImpl_ClassName<C>::ArrayPointerImpl_ClassName()
{
    num = max = 0;
    items = NULL;
}

template <class C>
ArrayPointerImpl_ClassName<C>::~ArrayPointerImpl_ClassName()
{
    //reset array
    Reset();
}

template <class C>
C *ArrayPointerImpl_ClassName<C>::Get(int32 index)
{
    IFBREAKNULL(index < 0 || index >= num);

    return items[index];
}

template <class C>
C const *ArrayPointerImpl_ClassName<C>::Get(int32 index) const
{
    IFBREAKNULL(index < 0 || index >= num);

    return items[index];
}

template <class C>
int32 ArrayPointerImpl_ClassName<C>::Num() const
{
    return num;
}

template <class C>
C const *const *ArrayPointerImpl_ClassName<C>::Array() const
{
    return items;
}

template <class C>
void ArrayPointerImpl_ClassName<C>::TakeFrom(ArrayPointerImpl_ClassName &other)
{
    //clear out our items
    Reset();

    //take data from other one
    items = other.items;
    num = other.num;
    max = other.max;

    //remove from other list
    other.items = null;
    other.num = other.max = 0;
}

template <class C>
void ArrayPointerImpl_ClassName<C>::Add(C *item)
{
    IFBREAKRETURN(item == NULL);

    //check if we need to lengthen the array
    if (num >= max)
    {
        //lengthen it.
        IFBREAKRETURN(Lengthen() == false);
    }

    //insert the item in the last slot
    items[num] = item;

    //we have one more item now
    num++;
}

template <class C>
C *ArrayPointerImpl_ClassName<C>::Remove(int32 index)
{
    //check the number.
    IFBREAKNULL(index < 0 || index >= num);

    //get the item we are about to remove
    C *item = items[index];

    #ifdef ArrayPointerImpl_Queue
    //move all elements after this one up one index
    for (int32 i = index + 1; i < num; i++)
    {
        //move this element up
        items[i - 1] = items[i];
    }
    #else
    //move the last element to this slot
    items[index] = items[num - 1];
    #endif

    //we now have 1 less item.
    num--;

    //return the item
    return item;
}

template <class C>
C *ArrayPointerImpl_ClassName<C>::Remove(C *item)
{
    IFBREAKNULL(item == NULL);

    //search through the array
    for (int32 i = 0; i < num; i++)
    {
        //check if the item is here
        if (items[i] == item)
        {
            //remove it
            return Remove(i);
        }
    }

    //never found it
    return NULL;
}

template <class C>
void ArrayPointerImpl_ClassName<C>::Reset()
{
    //empty the array first
    Empty();

    //delete the array
    delca(items);

    //we have no array anymore
    max = 0;
}

template <class C>
void ArrayPointerImpl_ClassName<C>::Empty()
{
    #ifdef ArrayPointerImpl_Owner
    //delete each item in the array
    for (int32 i = 0; i < num; i++)
    {
        //delete this item
        delc(items[i]);
    }
    #endif

    //we have no items
    num = 0;
}

template <class C>
bool ArrayPointerImpl_ClassName<C>::Lengthen()
{
    //if we have no items, start with a length of 4
    if (max == 0)
    {
        //make the new array
        items = new C *[4];
        max = 4;

        //done
        return true;
    }

    //make a new array double the length
    C **new_items = new C *[max * 2];
    IFBREAKFALSE(new_items == NULL);

    //copy the old items
    for (int32 i = 0; i < max; i++)
    {
        new_items[i] = items[i];
    }

    //we have 2 times max now
    max *= 2;

    //delete the old array
    delca(items);

    //swap in the new array
    items = new_items;

    //success
    return true;
}

#if defined(ArrayPointerImpl_Sorted)
template <class C>
void ArrayPointerImpl_ClassName<C>::Add(C *item, CompareFunc compareFunc)
{
    IFBREAKRETURN(item == NULL);
    IFBREAKRETURN(compareFunc == NULL);

    //check if there are no items
    if (num < 1)
    {
        //just add it
        Add(item);

        //done
        return;
    }

    //check if it belongs before the first item
    if (compareFunc((C const **)&item, (C const **)&items[0]) < 0)
    {
        //it goes at the very beginning
        Insert(item, (int32)0);
        
        //done
        return;
    }

    //check if it belongs after the last one
    if (compareFunc((C const **)&item, (C const **)&items[num - 1]) > 0)
    {
        //it goes after end
        Insert(item, num);

        //done
        return;
    }

    //it goes somewhere after index 0 and before index num
    int32 low = 0;
    int32 high = num;

    //bring bounds closer until they're touching
    while (high - low > 1)
    {
        //find index between them
        int32 middle = (high + low) / 2;

        //compare our item to the middle item
        int32 relation = compareFunc((C const **)&item, (C const **)&items[middle]);

        //check if it is before
        if (relation < 0)
        {
            //the item goes in first half
            high = middle;

            //start over
            continue;
        }

        //check if it is after
        if (relation > 0)
        {
            //the item goes in last half
            low = middle;

            //start over
            continue;
        }

        //bad news, one already here
        BREAK1();
        return;
    }

    //item goes between low and high index, insert at high to move it down
    Insert(item, high);
}

template <class C>
bool ArrayPointerImpl_ClassName<C>::Insert(C *item, int32 index)
{
    IFASSERTFALSE(item == NULL);
    IFASSERTFALSE(index < 0 || index > num);

    //add the element at the end, which extends the array if necessary
    Add(item);

    //check if we were inserting at the end
    if (index == num - 1)
    {
        //its already in the exact spot we want it
        return true;
    }

    //move all items down one spot up to and including the given index
    for (int32 i = num - 2; i >= index; i--)
    {
        //move this item down
        items[i + 1] = items[i];
    }

    //put the item at the spot we want
    items[index] = item;

    //success
    return true;
}


template <class C> template <class ID>
C *ArrayPointerImpl_ClassName<C>::Find(ID const id, typename FindFunc<ID>::Func findFunc)
{
    IFBREAKNULL(id == NULL || findFunc == NULL);

    //check if we have no items
    if (num < 1)
    {
        //wont find anything
        return NULL;
    }

    //the item is in some index between low and high, includsive
    int32 low = 0;
    int32 high = num - 1;

    //search until we're down to no items
    while (low <= high)
    {
        //get the index in the middle
        int32 middle = (low + high) / 2;

        //check relation of the id we're looking for and the middle item
        int32 relation = findFunc(id, items[middle]);

        //check if item with that id is before middle
        if (relation < 0)
        {
            //its in first half
            high = middle - 1;
            continue;
        }

        //check if item with that id is after middle
        if (relation > 0)
        {
            //its in last half
            low = middle + 1;
            continue;
        }

        //the item we want is right here at middle index
        return items[middle];
    }

    //didnt find it anywhere
    return NULL;
}

template <class C>
C *ArrayPointerImpl_ClassName<C>::Find(C const *item, CompareFunc compareFunc)
{
    IFBREAKNULL(item == NULL || compareFunc == NULL);

    //check if we have no items
    if (num < 1)
    {
        //wont find anything
        return NULL;
    }

    //the item is in some index between low and high, includsive
    int32 low = 0;
    int32 high = num - 1;

    //search until we're down to no items
    while (low <= high)
    {
        //get the index in the middle
        int32 middle = (low + high) / 2;

        //check relation of the item we're looking for and the middle item
        int32 relation = compareFunc((C const **)&item, (C const **)&items[middle]);

        //check if the given item is before the middle
        if (relation < 0)
        {
            //its in first half
            high = middle - 1;
            continue;
        }

        //check if the given item is after the middle
        if (relation > 0)
        {
            //its in last half
            low = middle + 1;
            continue;
        }

        //the item we want is right here at middle index
        return items[middle];
    }

    //didnt find it anywhere
    return NULL;
}

template <class C> template <class ID>
C *ArrayPointerImpl_ClassName<C>::Remove(ID const id, typename FindFunc<ID>::Func findFunc)
{
    IFBREAKNULL(id == NULL || findFunc == NULL);

    //check if we have no items
    if (num < 1)
    {
        //wont find anything
        return NULL;
    }

    //the item is in some index between low and high, inclusive
    int32 low = 0;
    int32 high = num - 1;

    //search until we're down to no items
    while (low <= high)
    {
        //get the index in the middle
        int32 middle = (low + high) / 2;

        //check relation of the id we're looking for and the middle item
        int32 relation = findFunc(id, (C const **)&items[middle]);

        //check if item with that id is before middle
        if (relation < 0)
        {
            //its in first half
            high = middle - 1;
            continue;
        }

        //check if item with that id is after middle
        if (relation > 0)
        {
            //its in last half
            low = middle + 1;
            continue;
        }

        //the item we want is right here at middle index
        C *item = items[middle];

        //remove it from the array
        ((Super *)this)->Remove(middle);

        //return the item
        return item;
    }

    //didnt find it anywhere
    return NULL;
}
#endif


#undef ArrayPointerImpl_ClassName
#undef ArrayPointerImpl_Owner
#undef ArrayPointerImpl_Queue
#undef ArrayPointerImpl_Sorted
