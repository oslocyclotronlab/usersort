/* -*- c++ -*-
 * STFileBufferFetcher.h
 *
 *  Created on: 10.03.2010
 *      Author: Alexander Bürger
 */

#ifndef STFILEBUFFERFETCHER_H_
#define STFILEBUFFERFETCHER_H_

#include "FileBufferFetcher.h"
#include "FileReader.h"
#include "Buffer.h"

#include <string>

//! Class for fetching buffers from a file.
/*! This class does not prefetch buffers or spawn a reader thread.
 */
class STFileBufferFetcher : public FileBufferFetcher {
public:

    /*! Calls the reader to open a file. */
    Status Open(const std::string& filename, int bufnum)
        { return reader.Open(filename, bufnum*buffer.GetSize()*4) ? OKAY : ERROR; }

    /*! Calls the reader to fetch a buffer. */
    const Buffer* Next(Status& state);

private:
    //! The class implementing the actual reading.
    FileReader reader;

    //! The buffer used to store the file data in.
    SiriusBuffer buffer;
};

#endif /* STFILEBUFFERFETCHER_H_ */
