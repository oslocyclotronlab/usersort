/* -*- c++ -*-
 * FileReader.h
 *
 *  Modified on: 05.09.2011
 *      Author: Alexander Bürger
 */

#ifndef FILEREADER_H_
#define FILEREADER_H_

#include <string>
#include <cstdio>
#ifndef MISSING_ZLIB
#include <zlib.h>
#endif

class Buffer;

//! Class for reading buffers from a file.
/*! This class performs the actual reading for both
 *  STFileBufferFetcher and MTFileBufferFetcher.
 *  
 * It can be compiled to read both files compressed with gzip
 * (filename ending with <code>.gz</code>) and not compressed files
 * (any other ending).
 */
class FileReader {
public:
    FileReader();

    //! Close file if still open.
    ~FileReader();

    //! Open a file and go to the specified position.
    /*! \return true if both opening and seeking were successful.
     */
    bool Open(const std::string& filename, /*!< The name of the file to open. */
              unsigned int seekpos         /*!< At which byte to position for reading. */);

    //! Read a buffer from the file.
    /*! \return 1 for new buffer, 0 for end of file, -1 for error.
     */
    int Read(char* data,       /*!< The buffer to sore the file data into. */
             unsigned int size /*!< how many bytes to read. */);

    //! Retrieve error flag.
    /*! \return The error flag.
     */
    bool IsError() const
        { return errorflag; }

private:
    //! Close the file, reset the error flag.
    void Close();

    //! The object for reading uncompressed files.
    std::FILE* file_stdio;

#ifndef MISSING_ZLIB
    //! The object for reading gzip'ed files.
    gzFile     file_gz;
#endif

    //! The error flag.
    bool errorflag;
};

#endif /* FILEREADER_H_ */
