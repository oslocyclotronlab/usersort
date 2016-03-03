/* -*- c++ -*-
 * OfflineSorting.h
 *
 *  Created on: 10.03.2010
 *      Author: Alexander Bürger
 */

#ifndef OFFLINESORTING_H_
#define OFFLINESORTING_H_

#include "RateMeter.h"
#include "Unpacker.h"
#include "Event.h"

#include "aptr.h"
#include <string>

class FileBufferFetcher;
class UserRoutine;

//! A class to make an offline sorting.
class OfflineSorting {
public:
    //! Initialize.
    /*! By default, no maximum buffer number is set and the files are
     *  read using a MTFileBufferFetcher.
     */
    OfflineSorting(UserRoutine& us /*!< The user sorting routine to use. */);

    //! Run all the commands in the batch file.
    void Run(const std::string& batchfilename /*!< The name of teh batch file to process. */);

    //! Convenience helper for a short main() routine.
    /*! It can be used like this:
     *  <pre>
     *  int main(int argc, char* argv[])
     *  {
     *      return OfflineSorting::Run(new UserXY(), argc, argv );
     *  }
     *  </pre>
     */
    static int Run(UserRoutine* us, int argc, char* argv[]);

    //! Set the maximum number of buffers to be read per file.
    void SetMaxBuffers(int maxBuffers /*!< The maximum buffer count. */);

    //! Set the object sed to fetch buffers.
    void SetBufferFetcher(FileBufferFetcher* bf /*!< The object used to read the data files. */);

    //! Sort one file.
    /*! \return true if all was okay.
     */
    bool SortFile(const std::string& filename, /*!< The name of the file to read. */
                  int begin,                   /*!< The first buffer to read. */
                  int end                      /*!< The last buffer to read. */ );

protected:
    //! Sort one buffer.
    /*! \return true if all was okay.
     */
    bool SortBuffer(const Buffer* buffer /*<! The buffer to sort. */);

private:
    //! The user routine for actually making the spectra.
    /*! \return true if all was okay.
     */
    UserRoutine& userRoutine;

    //! Handles 'export' commands.
    bool export_command(std::istream& icmd);

    //! Handles 'data' commands.
    /*! Reads the parameters and calls SortFile().
     *
     * \return true if all was okay.
     */
    bool data_command(std::istream& icmd /*!< The part of the command after 'data'. */ );

    //! Handle a command.
    /*! \return true if all was okay.
     */
    bool next_command(const std::string& cmd /*!< The command. */);

    //! Get the next command from the input stream.
    /*! The input will normally be the batch file.
     *
     * \return true if all was okay.
     */
    bool next_commandline(std::istream& in, std::string& cmd_line);

    //! True if the standard output is a terminal.
    /*! If writing to a terminal, we print info more frequently and we
     *  clear the output line sometimes.
     */
    bool is_tty;

    //! The prefix for paths given in "data file" commands.
    std::string data_directory;

    //! The maximum number of buffers to read from each file.
    int maxBuffers;

    //! The object used to read the files.
    aptr<FileBufferFetcher> bufferFetcher;

    //! The ratemeter for the buffer rate measurement.
    RateMeter rateMeter;

    //! The object performing the unpacking of events.
    Unpacker unpack;

    //! The event structure used for unpacking and sorting.
    Event event;
};

#endif /* OFFLINESORTING_H_ */
