
// -*- c++ -*-

#ifndef APTR_CXX
#define APTR_CXX

#include "aptr.h"

template<typename T>
void aptr<T>::reset(T* p=0)
{
    if (p!=ptr) {
        delete ptr;
        ptr=p;
    }
}

template<typename T>
T& aptr<T>::operator*() const
{
    return *ptr;
}

#endif
