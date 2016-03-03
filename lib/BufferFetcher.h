/* -*- c++ -*-
 * BufferFetcher.h
 *
 *  Created on: 10.03.2010
 *      Author: ab
 */

#ifndef BUFFERFETCHER_H_
#define BUFFERFETCHER_H_

#include "Buffer.h"

//! Interface of classes that are able to fetch buffers for sorting.
class BufferFetcher {
public:
    typedef enum { OKAY, //!< Buffer was fetched without problems.
        END,             //!< End of buffer stream was reached.
        ERROR,           //!< Error while trying to fetch buffer.
        WAIT             //!< A buffer might be available later.
    } Status;

    //! Fetch the next buffer.
    /*! \return OKAY if buffer was fetched, END if there are no more buffers
     *          ERROR in case of error, WAIT if fetching a buffer might be possible later
     */
    virtual const Buffer* Next(Status& state /*!< Will contain the status after reading. */) = 0;

    //! Virtual no-op destructor.
    virtual ~BufferFetcher() { };
};

#endif /* BUFFERFETCHER_H_ */
