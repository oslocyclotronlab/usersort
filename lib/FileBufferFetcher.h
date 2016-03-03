/* -*- c++ -*-
 * FileBufferFetcher.h
 *
 *  Created on: 10.03.2010
 *      Author: Alexander Bürger
 */

#ifndef FILEBUFFERFETCHER_H_
#define FILEBUFFERFETCHER_H_

#include "BufferFetcher.h"

#include <string>

//! Interface for fetching buffers from a file.
class FileBufferFetcher : public BufferFetcher {
public:
    //! Open a new file.
    /*! If a file was open previously, it should be closed.
     * 
     *  \return the status after opening the file.
     */
    virtual Status Open(const std::string& filename, /*!< The name of the file to open.    */
                       int bufnum                   /*!< The buffer number to start from. */) = 0;
};

#endif /* FILEBUFFERFETCHER_H_ */
