/* -*- c++ -*-
 * Buffer.h
 *
 *  Created on: 10.03.2010
 *      Author: Alexander Bürger
 */

#ifndef BUFFER_H_
#define BUFFER_H_

//! An event buffer.
class Buffer {
protected:

    //! Initialize a buffer with a given size and data buffer.
    Buffer(unsigned int sz, unsigned int* bffr)
        : size( sz ), buffer( bffr ) { }

    void SetBuffer(unsigned int* bffr)
        { buffer = bffr; }

public:
    //! Virtual no-op destructor.
    virtual ~Buffer() { }

    //! Get the size of the buffer in int words.
    unsigned int GetSize() const
        { return size; };

    //! Get data from the buffer.
    /*! \return The data word from the buffer.
     */
    unsigned int GetData(int idx /*!< The index of the data word to retrieve. */) const
        { return buffer[idx]; }

    //! Convenience function for GetData().
    int operator[](int idx) const
        { return GetData(idx); }

    //! Get access to the buffer memory.
    /*! This is to be used by classes fetching buffers,
     *  e.g. MTFileBufferFetcher.
     */
    unsigned int* GetBuffer()
        { return buffer; }

    //! Create a new buffer of the same type.
    /*! \return a new buffer, or 0
     */
    virtual Buffer* New() { return 0; }

private:
    //! The buffer size.
    unsigned int size;

    //! The buffer data.
    unsigned int* buffer;
};

// ########################################################################
// ########################################################################

//! A sirius event buffer with 128kB size (32768 words).
class SiriusBuffer : public Buffer {
    enum { BUFSIZE = 0x8000 /*!< The size of a sirius buffer in words. */ };
public:
    SiriusBuffer() : Buffer(BUFSIZE, new unsigned int[BUFSIZE]) { }
    ~SiriusBuffer() { delete GetBuffer(); }
    Buffer* New() { return new SiriusBuffer(); }
};

#endif /* BUFFER_H_ */
