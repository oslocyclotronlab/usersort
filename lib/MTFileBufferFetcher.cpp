/*
 * MTFileBufferFetcher.cpp
 *
 *  Created on: 10.03.2010
 *      Author: Alexander Bürger
 */

#include "MTFileBufferFetcher.h"

#include "aptr.ipp"
#include "FileReader.h"
#include "Buffer.h"

#include <cstdlib>
#include <iostream>
#include <pthread.h>

#define NDEBUG 1
#include "debug.h"

// ########################################################################
// ########################################################################

//! A helper class for handling pthread mutex objects.
class PThreadMutex {
public:
    //! Initialize the mutex.
    PThreadMutex()
        { pthread_mutex_init( &mutex, 0 ); }

    //! Finalize the mutex.
    ~PThreadMutex()
        { pthread_mutex_destroy( &mutex ); }

    //! Lock the mutex.
    void Lock()
        { pthread_mutex_lock( &mutex ); }

    //! Unlock the mutex.
    void Unlock()
        { pthread_mutex_unlock( &mutex ); }

    //! Wait for a condition.
    void Wait( pthread_cond_t* cond /*!< The condition to wait for. */)
        { pthread_cond_wait( cond, &mutex ); }

private:
    // disabled, not implemented
    PThreadMutex(const PThreadMutex& other);
    PThreadMutex& operator=(const PThreadMutex& other);

    //! The pthread mutex.
    pthread_mutex_t mutex;
};

// ########################################################################
// ########################################################################

//! A helper class for unlocking a mutex.
class PThreadMutexLock {
public:
    //! Lock the mutex.
    PThreadMutexLock(PThreadMutex& mtx /*!< The mutex to be locked. */)
        : mutex(mtx) { mutex.Lock(); }
    
    //! Unlock the mutex.
    ~PThreadMutexLock()
        { mutex.Unlock(); }
    
private:
    // disabled, not implemented
    PThreadMutexLock(const PThreadMutexLock& other);
    PThreadMutexLock& operator=(const PThreadMutexLock& other);

    //! The mutex to be locked and unlocked.
    PThreadMutex& mutex;
};

// ########################################################################
// ########################################################################

//! A ring buffer.
/*! The implementation is messy.
 */
template<class T, int N, bool created>
class RingBuffer {
public:
    RingBuffer();
    ~RingBuffer();

    bool Full() const { return size==N; }
    bool Empty() const { return size==0; }
    T* Put(T* t=dummy);
    T* Get();
    void Set(unsigned int idx, T* t)
        { ring[idx] = t; }

private:
    static T* dummy;

    T* ring[N];
    int head, tail, size;
};

// ########################################################################

template<class T, int N, bool created>
RingBuffer<T,N,created>::RingBuffer()
    : head(0), tail(0), size(0)
{
    for(int i=0; i<N; ++i)
        ring[i] = 0;
}

// ########################################################################

template<class T, int N, bool created>
RingBuffer<T,N,created>::~RingBuffer()
{
    if( created ) {
        for(int i=0; i<N; ++i)
            delete ring[i];
    }
}

// ########################################################################

template<class T, int N, bool created>
T* RingBuffer<T,N,created>::Put(T* t)
{
    if( Full() )
        return 0;
    if( created ) {
        t = ring[head];
    } else {
        ring[head] = t;
    }
    head = (head+1)%N;
    size += 1;
    return t;
}

// ########################################################################

template<class T, int N, bool created>
T* RingBuffer<T,N,created>::Get()
{
    if( Empty() )
        return 0;
    T* r = created ? 0 : ring[tail];
    tail = (tail+1)%N;
    size -= 1;
    return r;
}

// ########################################################################

template<class T, int N, bool created>
T* RingBuffer<T,N,created>::dummy = 0;

// ########################################################################
// ########################################################################

//! Class used by MTFileBufferFetcher to read buffers in a separate thread.
class PrefetchThread {
public:
    //! Initialize, but do not yet start running.
    PrefetchThread(FileReader* reader,     /*!< Helper to perform the actual file reading. */
                   Buffer* template_buffer /*!< Buffer object to be "multiplied". */);

    //! Cleanup after the thread stopped running.
    ~PrefetchThread();

    //! Start the new thread.
    void Start();

    //! Called to get a new buffer for sorting.
    Buffer* ReadingBegins();

    //! Called after sorting a buffer has finished.
    void ReadingEnds();

    //! Stop the thread.
    void Stop();

private:
    //! The main loop of the thread.
    void StartReading();

    //! Helper for pthread_create.
    static void* Run(void* v)
        { ((PrefetchThread*)v)->StartReading(); return 0; }

    //! The mutex for synchonizing acces to the ring buffers.
    PThreadMutex mutex;

    //! The condition "write ring full".
    pthread_cond_t cond_full;

    //! The condition "read ring not empty".
    pthread_cond_t cond_avail;

    //! The thread object;
    pthread_t thread;
    
    //! The file reading implementation.
    FileReader* reader;

    enum { NBUFFERS = 8 /*!< By default, read up to 8 buffers in advance. */ };

    //! The ring for fetching buffers.
    RingBuffer<Buffer,NBUFFERS,true> writeRing;

    //! The ring for reading buffers.
    RingBuffer<Buffer,NBUFFERS,false> readRing;

    //! Flag set to stop the thread. Only written by main thread.
    bool cancel;

    //! Flag that reading the file is finished. Only written by the prefetch thread.
    bool finished;
};

// ########################################################################

PrefetchThread::PrefetchThread(FileReader* rdr, Buffer* template_buffer)
    : reader( rdr )
    , cancel( false )
    , finished( false )
{
    pthread_cond_init( &cond_full,  0 );
    pthread_cond_init( &cond_avail, 0 );

    for(int i=0; i<NBUFFERS; ++i)
        writeRing.Set(i, template_buffer->New());
}

// ########################################################################

void PrefetchThread::Start()
{
    if( pthread_create( &thread, NULL, PrefetchThread::Run, this ) != 0 ) {
        std::cerr << "cannot create reader thread." << std::endl;
        exit( -1 );
    }
}

// ########################################################################

void PrefetchThread::ReadingEnds()
{
    PThreadMutexLock lock( mutex );
    writeRing.Get();
    pthread_cond_signal( &cond_full );
}

// ########################################################################

Buffer* PrefetchThread::ReadingBegins()
{
    PThreadMutexLock lock( mutex );
    while( true ) {
        if( finished && readRing.Empty() )
            return 0;
        if( readRing.Empty() ) {
            mutex.Wait( &cond_avail );
            continue;
        }
        return readRing.Get();
    }
}

// ########################################################################

void PrefetchThread::StartReading()
{
    while( !cancel && !finished ) {
        Buffer* buffer = 0;
        { // critical section
            PThreadMutexLock lock( mutex );
            while( !cancel && !finished && writeRing.Full() )
                mutex.Wait( &cond_full );
            if( cancel || finished )
                return;
        
            // claim a buffer, but do not yet say it is filled
            buffer = writeRing.Put();
        } // unlock in 'lock' destructor

        // reading is time-consuming and should be performed while the
        // lock is released
        if( reader->Read((char*)buffer->GetBuffer(), buffer->GetSize()*4 ) <= 0 )
            finished = true;

        { // critical section
            PThreadMutexLock lock( mutex );
            if( !finished ) {
                // mark the buffer as readable
                readRing.Put(buffer);
            } else {
                // would be wrong if > 1 thread; fortunately we have
                // only 1; for two threads T1 and T2, if T1 finishes
                // the previous critical section first, then T2 runs
                // that section, reads a buffer before T1, and detects
                // end of file, then T2 would "free" the buffer taken
                // by T1
                writeRing.Get();
            }
            // tell main thread that data are available
            pthread_cond_signal( &cond_avail );
        } // unlock in 'lock' destructor
    }
}

// ########################################################################

void PrefetchThread::Stop()
{
    { // critical section
        PThreadMutexLock lock( mutex );
        cancel = true;
        pthread_cond_signal( &cond_full );
    } // unlock in 'lock' destructor

    // wait for thread to terminate
    pthread_join( thread, NULL );
}

// ########################################################################

PrefetchThread::~PrefetchThread()
{
    pthread_cond_destroy( &cond_full );
    pthread_cond_destroy( &cond_avail );
}

// ########################################################################
// ########################################################################

MTFileBufferFetcher::MTFileBufferFetcher()
    : reader( new FileReader() )
    , template_buffer( new SiriusBuffer() )
    , prefetch( 0 )
{
}

// ########################################################################

MTFileBufferFetcher::~MTFileBufferFetcher()
{
    StopPrefetching();
}

// ########################################################################

const Buffer* MTFileBufferFetcher::Next(Status& state)
{
    if( reader->IsError() ) {
        state = ERROR;
        return 0;
    }

    if( !prefetch ) {
        prefetch = new PrefetchThread( reader.get(), template_buffer.get() );
        prefetch->Start();
    } else {
        // finish reading the buffer from the last call to Next()
        prefetch->ReadingEnds();
    }

    // fetch the next buffer
    const Buffer* b = prefetch->ReadingBegins();
    state = b ? OKAY : END;
    return b;
}

// ########################################################################

BufferFetcher::Status MTFileBufferFetcher::Open(const std::string& filename, int bufnum)
{
    StopPrefetching();
    int i = reader->Open( filename, bufnum*template_buffer->GetSize()*4 );
    if( i>0 ) return OKAY; else if( i==0 ) return END; else return ERROR;
}

// ########################################################################

void MTFileBufferFetcher::StopPrefetching()
{
    if( !prefetch )
        return;

    prefetch->Stop();
    delete prefetch;
    prefetch = 0;
}
