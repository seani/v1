#pragma once

#include "common\global\global.h"

//
//ArrayPointer is the most basic array type.  It holds pointers to objects
//of its template parameter type.  The implementation is a dynamically allocated
//array of pointers, which will double in size every time it is filled up.
//
//Insertion is done at the end of the array and is O(1) when the array does 
//not need to resize, and O(n) when the array is resized.  Overall, insertion is
//O(log(n)) time.
//
//There can never be a NULL pointer in the array.  Removal of an object from 
//the array will cause a gap to occur which is filled with the element from the end
//of the array.  Removal is O(1).
//
#define ArrayPointerImpl_ClassName ArrayPointer
#include "src\ArrayPointerImpl.h"

//
//ArrayPointerOwner differs from ArrayPointer only in that it will delete
//it's items on Empty and Reset, which become O(n).
//
#define ArrayPointerImpl_ClassName ArrayPointerOwner
#define ArrayPointerImpl_Owner
#include "src\ArrayPointerImpl.h"

//
//ArrayPointerQueue preserves the order of items when calling Remove, which makes it O(n)
//
#define ArrayPointerImpl_ClassName ArrayPointerQueue
#define ArrayPointerImpl_Queue
#include "src\ArrayPointerImpl.h"

//
//Combines changes in ArrayPointerOwner and ArrayPointerQueue
//
#define ArrayPointerImpl_ClassName ArrayPointerOwnerQueue
#define ArrayPointerImpl_Owner
#define ArrayPointerImpl_Queue
#include "src\ArrayPointerImpl.h"

//
//Keeps the array in sorted order to provide O(log(n)) searching.
//Inserts and removes are still O(n).
//
#define ArrayPointerImpl_ClassName ArraySorted
#define ArrayPointerImpl_Sorted
#include "src\ArrayPointerImpl.h"

//
//Same as ArraySorted, but owns the items and will delete them when appropriate.
//
#define ArrayPointerImpl_ClassName ArraySortedOwner
#define ArrayPointerImpl_Owner
#define ArrayPointerImpl_Sorted
#include "src\ArrayPointerImpl.h"


//
//These macros will generate simple static Compare and Find functions that can be used in the sorted array classes.
//

#define SortedArrayCompareFuncMember(funcName, Type, memberVar) static inline int32 funcName(Type const **left, Type const **right) { return ::CompareFunc((*left)->memberVar, (*right)->memberVar); };
#define SortedArrayFindFuncMember(funcName, _IDType, _ItemType, memberVar) static inline int32 funcName(_IDType const id, _ItemType const *item) { return ::CompareFunc(id, item->memberVar); };
#define SortedArrayFunctions(compareFuncName, findFuncName, _ItemType, _IDType, memberVar) SortedArrayCompareFuncMember(compareFuncName, _ItemType, memberVar) SortedArrayFindFuncMember(findFuncName, _IDType, _ItemType, memberVar)

//generic compare template function for integral types
template <class Type> int32 CompareFunc(Type const left, Type const right)
{
    //compare them with operators
    if (left < right)
    {
        //left is less
        return -1;
    }
    else if (left > right)
    {
        //left is more
        return 1;
    }
    else
    {
        //they are equal
        return 0;
    }
}

//first class function overloads for strings instead of the above template.
inline int32 CompareFunc(wchar const *left, wchar const *right)
{
    //use standard string compare
    return (int32)wcscmp(left, right);
}
inline int32 CompareFunc(char const *left, char const *right)
{
    //use standard string compare
    return (int32)strcmp(left, right);
}

