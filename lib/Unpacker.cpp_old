/*
 * Unpacker.cpp
 *
 *  Created on: 10.03.2010
 *      Author: ab
 */

#include "Unpacker.h"

#include "Buffer.h"
#include "Event.h"

#define NDEBUG 1
#include "debug.h"

inline unsigned int boe(unsigned int x) { return ((x&0xC0000000)>>28); }
inline unsigned int ndw(unsigned int x) { return  (x&0x000000ff);      }
inline unsigned int box(unsigned int x) { return ((x&0x3f800000)>>23); }
inline unsigned int chn(unsigned int x) { return ((x&0x007f0000)>>16); }
inline unsigned int dta(unsigned int x) { return  (x&0x0000ffff);      }

//#define BYTESWAP_WORDS 1 // for PowerPC
#ifdef BYTESWAP_WORDS
inline unsigned int byteswap(unsigned int u)
{ return ((u&0xff)<<24)|((u&0xff00)>>8)|((u&0xff0000)>>8)|((u&0xff000000)>>24); }
#else
inline unsigned int byteswap(unsigned int u)
{ return u; }
#endif

static const unsigned int EOB = 0x80000000;

// ########################################################################

bool Unpacker::UnpackOneEvent(unsigned int n_data, Event& event)
{
    // a map of CACTUS ids to indexes in the event's raw_na list
    int na_idx[32];
    for(int i=0; i<32; ++i)
        na_idx[i] = -1;

    // go through all data of this event
    for( unsigned int i=1; i<=n_data; i++ ) {
        const unsigned int l_word = byteswap((*buffer)[buffer_idx + i]);
        const int nbox = box(l_word); // the box that has fired

        if( boe(l_word) != 0 )
            // somehow a header word came into the event, maybe engine has overwritten the buffer?
            return false;

        if(nbox == 0x00) {        /* TPU pattern ch 0 - 3 */
            if( chn(l_word) == 0 && !event.has_pattern ) {
                event.has_pattern = true;
                event.pattern = dta(l_word);
            } else {
                return ERROR;
            }
        } else if(nbox == 0x01) { /* Wall-clock time  ch 16 (high) and 17 (low) */
            if( chn(l_word) != 16 || i>=n_data )
                return ERROR;
            const unsigned int l_word2 = (*buffer)[buffer_idx + i + 1];
            if( chn(l_word2) != 17 )
                return ERROR;
            event.has_time = true;
            event.time = ((long)dta(l_word))<<16 | dta(l_word2);
        } else if( nbox == 0x02) { /* VME scaler 1151N ch 0-15 (lo) and 16-31 (hi) */
            if( i>=n_data )
                return ERROR;
            const unsigned int l_word2 = (*buffer)[buffer_idx + i + 1];
            if( chn(l_word2) - chn(l_word) != 16 )
                return ERROR;

            Event::chn_scaler& s = event.scaler[event.n_scaler++];
            s.chn    = chn(l_word);
            s.scaler = ((long)dta(l_word2))<<16 | dta(l_word);
        } else if( nbox == 0x10) { /* Time of NaI ch 0-31*/
            const int c = chn(l_word);
            int idx = na_idx[c];
            if( idx<0 ) {
                idx = na_idx[c] = event.n_na++;
                event.na[idx].chn = c;
                event.na[idx].adc = 0;
            }
            event.na[idx].tdc = dta(l_word);
        } else if( nbox == 0x20       /* Energy of NaI ch 0-31 */
                   || nbox == 0x24) { /* Energy   MADC ch 0-31 */
            const int c = chn(l_word);
            int idx = na_idx[c];
            if( idx<0 ) {
                idx = na_idx[c] = event.n_na++;
                event.na[idx].chn = c;
                event.na[idx].tdc = 0;
            }
            event.na[idx].adc = dta(l_word);
        } else if( nbox == 0x21) { /* Energy E ch 0-32 */
            Event::chn_adc& e = event.e[event.n_e++];
            e.chn = chn(l_word);
            e.adc = dta(l_word);
        } else if( nbox == 0x22) { /* Energy dE1 ch 0-31 */
            Event::chn_adc& de1 = event.de[event.n_de++];
            de1.chn = chn(l_word);
            de1.adc = dta(l_word);
        } else if( nbox == 0x23) { /* Energy dE2 ch 32-61 */
            Event::chn_adc& de2 = event.de[event.n_de++];
            de2.chn = chn(l_word) + 32;
            de2.adc = dta(l_word);
        } else {
            return false;
        }
    }
    return true;
}

// ########################################################################

Unpacker::Unpacker()
    : buffer( 0 )
    , buffer_idx( 0 )
    , eventlength_sum( 0 )
    , event_count( 0 )
{
}

// ########################################################################

void Unpacker::SetBuffer(const Buffer* buffr)
{
    buffer = buffr;
    buffer_idx = 0;
    
    event_count = eventlength_sum = 0;
}

// ########################################################################

Unpacker::Status Unpacker::Next(Event& event)
{
    DBGV(buffer);
    if( buffer_idx >= buffer->GetSize() )
        return END;

    event.Reset();

    const unsigned int event_header = byteswap((*buffer)[buffer_idx]);
    if( boe(event_header) != 0xC ) {
        // not begin of event; then it should be the end of the buffer
        if( event_header != EOB )
            // ouch, bad buffer
            return ERROR;

        // okay, the end-of-buffer marker was there
        return END;
    }

    const int n_data = ndw(event_header);

    eventlength_sum += n_data;
    event_count += 1;

    if( !UnpackOneEvent(n_data, event) )
        // unpacking error
        return ERROR;

    buffer_idx += n_data + 1;

    return OKAY;
}
