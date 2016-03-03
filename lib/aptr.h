// -*- c++ -*-

#ifndef APTR_H
#define APTR_H

//! Smart pointer helper.
template<typename T>
struct aptr_ref
{
    T* ptr;
    explicit
    aptr_ref(T* p): ptr(p) { }
};

//! Smart pointer.
/*! Takes ownership of the object it holds, i.e. the object is deleted
 *  in the smart pointer's destructor.
 */
template<typename T>
class aptr {
private:
    T* ptr;

public:
    aptr(T* p=0) : ptr(p) { }

    aptr(aptr& o) : ptr(o.release()) { }

    template<typename S>
    aptr(aptr<S>& s) : ptr(s.release()) { }

    aptr& operator=(aptr& o)
        { reset(o.release()); return *this; }

    template<typename S>
    aptr& operator=(aptr<S>& s)
        { reset(s.release()); return *this; }

    ~aptr() { reset(0); }

    operator bool() const { return (ptr!=0); }
    inline T& operator*() const;
    T* operator->() const { return ptr; }
    T* get() const { return ptr; }

    T* release() { T* p=ptr; ptr=0; return p; }

    void reset(T* p);


    aptr(aptr_ref<T> r) : ptr(r.ptr) { }

    aptr& operator=(aptr_ref<T> r)
        { reset( r.ptr ); return *this; }

    template<typename S>
    operator aptr_ref<S>() { return aptr_ref<S>(this->release()); }

    template<typename S>
    operator aptr<S>() { return aptr<S>(this->release()); }
};

#endif
