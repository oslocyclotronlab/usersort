/*
 * FileReader.cpp
 *
 *  Modified on: 05.09.2011
 *      Author: Alexander Bürger
 */

#include "FileReader.h"

#include "Buffer.h"

FileReader::FileReader()
    : file_stdio(0)
#ifndef MISSING_ZLIB
    , file_gz(0)
#endif
    , errorflag( true )
{
}

// ########################################################################

FileReader::~FileReader()
{
    Close();
}

// ########################################################################

int FileReader::Read(char* data, unsigned int size_req)
{
    if( errorflag || (!file_stdio
#ifndef MISSING_ZLIB
                      && !file_gz
#endif
            ) )
    {
        return -1;
    }

    unsigned int have = 0;
    while( have<size_req ) {
        int now = -1;
        if( file_stdio )
            now = std::fread(data+have, 1, size_req-have, file_stdio);
#ifndef MISSING_ZLIB
        else if( file_gz )
            now = gzread(file_gz, data+have, size_req-have);
#endif
        if( now<=0 ) {
            errorflag = (now<0 || (have!= 0 && have!=size_req));
            Close();
            return errorflag ? -1 : 0;
        } else {
            have += now;
        }
    }
    return 1;
}

// ########################################################################

bool FileReader::Open(const std::string & filename, unsigned int want)
{
    Close();
    if( filename.find(".gz") == filename.size()-3 ) {
#ifndef MISSING_ZLIB
        file_gz = gzopen(filename.c_str(), "rb");
        errorflag = (file_gz == 0)
            || gzseek(file_gz, want, SEEK_SET) != (int)want;
#else
        errorflag = true;
#endif
    } else {
        file_stdio = fopen(filename.c_str(), "rb");
        errorflag = (file_stdio == 0)
            || std::fseek(file_stdio, want, SEEK_SET) != (int)want;
    }
    return !errorflag;
}

// ########################################################################

void FileReader::Close()
{
    if( file_stdio ) {
        std::fclose(file_stdio);
        file_stdio = 0;
    }
#ifndef MISSING_ZLIB
    if( file_gz ) {
        gzclose(file_gz);
        file_gz = 0;
    }
#endif
}
