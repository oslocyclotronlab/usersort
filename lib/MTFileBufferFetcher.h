/* -*- c++ -*-
 * FileBufferFetcher.h
 *
 *  Created on: 10.03.2010
 *      Author: Alexander Bürger
 */

#ifndef MTFILEBUFFERFETCHER_H_
#define MTFILEBUFFERFETCHER_H_

#include "aptr.h"
#include "FileBufferFetcher.h"

#include <string>

class FileReader;

//! Fetch buffers from a file in a separate thread.
/*! This class spawns a thread to prefetch buffers on the first call
 *  to Next(). This is a bit more complicated, but it can also make
 *  the sorting 50% faster.
 */
class MTFileBufferFetcher : public FileBufferFetcher {
public:
    //! Construct the buffer fetcher.
    MTFileBufferFetcher();

    //! Closes the file, if still open.
    ~MTFileBufferFetcher();

    Status Open(const std::string& filename, int bufnum);

    /*! Creates a new thread which prefetches some buffers while the
     *  main thread is sorting.
     */
    const Buffer* Next(Status& state);

    //! Set the buffer template.
    void SetBuffer(Buffer* buf)
        { template_buffer.reset( buf ); }

private:
    //! Stop the prefetch thread.
    void StopPrefetching();

    aptr<FileReader> reader;

    aptr<Buffer> template_buffer;

    class PrefetchThread* prefetch;
};

#endif /* MTFILEBUFFERFETCHER_H_ */
