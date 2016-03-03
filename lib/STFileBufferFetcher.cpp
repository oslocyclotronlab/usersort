
#include "STFileBufferFetcher.h"
 
const Buffer* STFileBufferFetcher::Next(Status& state)
{
    int i = reader.Read( (char*)buffer.GetBuffer(), 4*buffer.GetSize() );
    if( i>0 )
        state=OKAY;
    else if( i==0 )
        state=END;
    else
        state=ERROR;
    return &buffer;
}
