/*
 * OfflineSorting.cpp
 *
 *  Created on: 10.03.2010
 *      Author: Alexander Bürger
 */

#include "OfflineSorting.h"

#include "Event.h"
#include "MTFileBufferFetcher.h"
#include "RateMeter.h"
#include "RootWriter.h"
#include "MamaWriter.h"
#include "STFileBufferFetcher.h"
#include "Unpacker.h"
#include "UserRoutine.h"

#include <fstream>
#include <iostream>
#include <signal.h>
#include <sstream>

#include "aptr.ipp"

#define NDEBUG 1
#include "debug.h"

static char leaveprog = 'n';

//! Signal handler for Ctrl-C.
static void keyb_int(int sig_num)
{
    if (sig_num == SIGINT) {
        printf("\n\nLeaving...\n");
        leaveprog = 'y';
    }
}

// ########################################################################
// ########################################################################
// ########################################################################

//! Remove whitespace at beginning and end.
/*! \return The text without leading and trailing whitespace.
 */
static std::string trim_whitespace( const std::string& text /*!< The text to clean. */ )
{
    size_t start = text.find_first_not_of(" \t");
    if( start==std::string::npos )
        return "";
    
    size_t end = text.find_last_not_of (" \t");
    if( end == std::string::npos || end<=start )
        end = text.size();
    return text.substr(start, end-start+1);
}

// ########################################################################
// ########################################################################
// ########################################################################

OfflineSorting::OfflineSorting(UserRoutine& us)
    : userRoutine( us )
    , is_tty( isatty(STDOUT_FILENO) )
    , maxBuffers(-1)
    , bufferFetcher(new MTFileBufferFetcher())
    , rateMeter(500, !is_tty)
{
    signal(SIGINT, keyb_int); // set up interrupt handler (Ctrl-C)
    signal(SIGPIPE, SIG_IGN);
}

// ########################################################################

void OfflineSorting::SetBufferFetcher(FileBufferFetcher* bf)
{
    bufferFetcher.reset( bf );
}

// ########################################################################

void OfflineSorting::SetMaxBuffers(int mb)
{
    maxBuffers = mb;
}

// ########################################################################

bool OfflineSorting::SortBuffer(const Buffer* buffer)
{
    unpack.SetBuffer(buffer);
        
    // unpack and sort all events from this buffer
    int unpack_err = Unpacker::END;
    while(leaveprog=='n') {
        unpack_err = unpack.Next(event);
        if( unpack_err != Unpacker::OKAY )
            break;
        userRoutine.Sort(event);
    }
    return unpack_err == Unpacker::END;
}

// ########################################################################

bool OfflineSorting::SortFile(const std::string& filename, int buf_start, int buf_end)
{
    // open data file
    if( bufferFetcher->Open(filename, buf_start) != BufferFetcher::OKAY ) {
        // TODO: exception
        std::cerr << "data: could not open '" << filename << "' or not seek to "
                  << buf_start << "'." << std::endl;
        return false;
    }

    int buffer_count = 0, bad_buffer_count = 0;
    rateMeter.Reset();

    // loop over all buffers
    for(int b=buf_start; buf_end<0 || b<buf_end; ++b) {
        // stop if Ctrl-C has been pressed
        if( leaveprog != 'n' )
            break;

        // fetch next buffer
        BufferFetcher::Status fstate;
        const Buffer* buf = bufferFetcher->Next(fstate);
        if( fstate == BufferFetcher::END ) {
            break;
        } else if( fstate == BufferFetcher::ERROR ) {
            // TODO exception
            std::cerr << "\ndata: error reading buffer " << b << std::endl;
            return false;
        }

        // sort buffer
        buffer_count += 1;
        const bool sort_ok = SortBuffer(buf);
        if( !sort_ok )
            bad_buffer_count += 1;

        // from time to time, print a message
        const float bufs_per_sec = rateMeter.Rate();
        if( bufs_per_sec > 0 ) {
            if( is_tty ) {
                std::cout << "        "  << std::flush << '\r' // clear the line
                          << buffer_count << '/' << bad_buffer_count
                          << ' ' << unpack.GetAverageLength()
                          << ' ' << bufs_per_sec << " bufs/s " << std::flush;
            } else {
                std::cout << '.' << std::flush;
            }
        }
    }

    // print counters and rate at the end
    std::cout << '\r' << buffer_count << '/' << bad_buffer_count
              << ' ' << unpack.GetAverageLength()
              << ' ' << rateMeter.TotalRate() << " bufs/s" << std::endl;
    return true;
}

// ########################################################################

bool OfflineSorting::data_command(std::istream& icmd)
{
    int buf_start=0, buf_end=maxBuffers;

    std::string tmp;
    icmd >> tmp;
    if( tmp == "directory" ) {
        icmd >> tmp;
        std::string dd = trim_whitespace( tmp );
        if( dd.empty() ) {
            std::cerr << "data: Do not understand directory '" << tmp << "'" << std::endl;
            return false;
        }
        data_directory = dd;
        std::cout << "Set data directory to '" << data_directory << "'" << std::endl;
        return true;
    }

    if( tmp == "buffers" ) {
        icmd >> buf_start >> buf_end >> tmp;
        if( maxBuffers>0 )
            buf_end = std::min(buf_end, buf_start+maxBuffers);
    }

    if( tmp != "file" ) {
        std::cerr << "data: Expected data [buffers <from> <to>] file <filename>.\n";
        return false;
    }

    // extract filename, trying to allow filenames with spaces inside
    std::string filenm;
    std::getline( icmd, filenm );
    std::string filename = trim_whitespace( filenm );
    if( filename.empty() ) {
        std::cerr << "data: Do not understand filename '" << filenm << "'" << std::endl;
        return false;
    }
    // prepend data_directory, but only for relative paths
    if( !data_directory.empty() && filename[0] != '/' )
        filename = data_directory + "/" + filename;

    // annouce what we will try to do
    std::cout << "data: reading file '" << filename
              << "' buffers [" << buf_start << ',';
    if( buf_end < 0 )
        std::cout << "end";
    else
        std::cout << buf_end;
    std::cout << "[." << std::endl;

    // and go!
    return SortFile(filename, buf_start, buf_end);
}

// ########################################################################

bool OfflineSorting::export_command(std::istream& icmd)
{
    std::string tmp;
    icmd >> tmp;
    if( tmp == "root" ) {
        icmd >> tmp;
        std::string rootfile = trim_whitespace( tmp );
        if( rootfile.empty() ) {
            std::cerr << "export root: do not understand ROOT filename '"
                      << rootfile << "'" << std::endl;
            return false;
        }
        std::cout << "export as ROOT file into '" << rootfile << "'" << std::endl;
        RootWriter::Write( userRoutine.GetHistograms(), rootfile );
        std::cout << "resetting all histograms" << std::endl;
        userRoutine.GetHistograms().ResetAll();
        return true;
    } else if( tmp == "mama" ) {
        icmd >> tmp;
        std::string histname = trim_whitespace( tmp );
        if( histname.empty() ) {
            std::cerr << "export mama: do not understand histogram name '"
                      << histname << "'" << std::endl;
            return false;
        }
        Histogram1Dp h = userRoutine.GetHistograms().Find1D( histname );
        Histogram2Dp m = userRoutine.GetHistograms().Find2D( histname );
        if( !m && !h ) {
            std::cerr << "export mama: no histogram named '"
                      << histname << "'" << std::endl;
            return false;
        }

        icmd >> tmp;
        std::string mamafile = trim_whitespace( tmp );
        if( mamafile.empty() ) {
            std::cerr << "export mama: do not understand mama filename '"
                      << mamafile << "'" << std::endl;
            return false;
        }
        std::ofstream mama_out( mamafile.c_str() );
        if( !mama_out ) {
            std::cerr << "export mama: problem opening '"
                      << mamafile << "'" << std::endl;
            return false;
        }
        std::cout << "export '" << histname << "' as MAMA file into '" << mamafile << "'" << std::endl;
        if( h )
            MamaWriter::Write( mama_out, h );
        else if( m )
            MamaWriter::Write( mama_out, m );
        if( !mama_out ) {
            std::cerr << "export mama: problem writing '"
                      << mamafile << "'" << std::endl;
            return false;
        }
        return true;
    }
    return false;
}

// ########################################################################

bool OfflineSorting::next_command(const std::string& cmd)
{
    std::istringstream icmd(cmd.c_str());

    std::string name, tmp;
    icmd >> name;

    if( name == "quit") {
        leaveprog = 'y';
        return true;
    } else if( name == "data" ) {
        return data_command(icmd);
    } else if( name == "export" ) {
        return export_command(icmd);
    } else if( name == "reset_histograms" ) {
        userRoutine.GetHistograms().ResetAll();
        return true;
    } else if( name == "max_buffers" ) {
        int mb;
        icmd >> mb;
        SetMaxBuffers(mb);
        return true;
    } else {
        return userRoutine.Command(cmd);
    }
}

// ########################################################################

bool OfflineSorting::next_commandline(std::istream& in, std::string& cmd_line)
{
    cmd_line = "";
    std::string line;
    while( getline(in, line) ) {
        int ls = line.size();
        if( ls==0 ) {
            break;
        } else if( line[ls-1] != '\\' ) {
            cmd_line += line;
            break;
        } else {
            cmd_line += line.substr(0, ls-1);
        }
    }
    return in || !cmd_line.empty();
}

// ########################################################################

void OfflineSorting::Run(const std::string& batchfilename)
{
    std::ifstream batch_file(batchfilename.c_str());
    std::string batch_line;
    while( leaveprog=='n' && next_commandline(batch_file, batch_line) ) {
        if( batch_line.size()==0 || batch_line[0] == '#' )
            continue;
        if( !next_command(batch_line) ) {
            std::cout << "Do not understand batch line '" << batch_line << "'" << std::endl;
            break;
        }
    }
}

// ########################################################################

int OfflineSorting::Run(UserRoutine* ur, int argc, char* argv[])
{
    if( argc != 2 ) {
        std::cerr << "Run like: " << argv[0] << " batchfile" << std::endl;
        return -1;
    }

    ur->Start();
    OfflineSorting offline( *ur );
    offline.Run( argv[1] );
    ur->End();
    delete ur;
    return 0;
}

// ########################################################################
// ########################################################################
// ########################################################################

/*! \mainpage

  You are looking at the documentation for the sirius sorting routines.

  This documentation was generated by running
<pre>
doxygen
</pre>

  You can use the documentation to read all the source code in your web browser.
  
  You should start by looking at the code in user_sort.cpp

  Good luck!
*/
