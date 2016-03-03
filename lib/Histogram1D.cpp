/*
 * Histogram1D.cpp
 *
 *  Created on: 10.03.2010
 *      Author: Alexander Bürger
 */

#include "Histogram1D.h"

#include <iostream>

#define NDEBUG
#include "debug.h"

#ifdef H1D_USE_BUFFER
const unsigned int Histogram1D::buffer_max;
#endif /* H1D_USE_BUFFER */

// ########################################################################

Histogram1D::Histogram1D( const std::string& name, const std::string& title,
                          int c, Axis::bin_t l, Axis::bin_t r, const std::string& xt )
    : Named( name, title )
    , xaxis( name+"_xaxis", c, l, r, xt )
    , data( 0 )
{
#ifdef H1D_USE_BUFFER
    buffer.reserve(buffer_max);
#endif /* H1D_USE_BUFFER */

    data = new data_t[xaxis.GetBinCountAll()];
    Reset();
}

// ########################################################################

Histogram1D::~Histogram1D()
{
    delete data;
}

// ########################################################################

void Histogram1D::Add(const Histogram1Dp other, data_t scale)
{
    if( !other 
        || other->GetName() != GetName()
        || other->GetAxisX().GetLeft() != xaxis.GetLeft()
        || other->GetAxisX().GetRight() != xaxis.GetRight()
        || other->GetAxisX().GetBinCount() != xaxis.GetBinCount() )
        return;

#ifdef H1D_USE_BUFFER
    other->FlushBuffer();
    FlushBuffer();
#endif /* H2D_USE_BUFFER */

    for(int i=0; i<xaxis.GetBinCountAll(); ++i)
        data[i] += scale * other->data[i];
}

// ########################################################################

Histogram1D::data_t Histogram1D::GetBinContent(int bin)
{
#ifdef H1D_USE_BUFFER
    FlushBuffer();
#endif /* H1D_USE_BUFFER */
    if( bin>=0 && bin<xaxis.GetBinCountAll() ) {
        return data[bin];
    } else {
        return 0;
    }
}

// ########################################################################

void Histogram1D::SetBinContent(int bin, data_t c)
{
#ifdef H1D_USE_BUFFER
    if( !buffer.empty() )
        FlushBuffer();
#endif /* H1D_USE_BUFFER */

    if( bin>=0 && bin<xaxis.GetBinCountAll() )
        data[bin] = c;
}

// ########################################################################

void Histogram1D::FillDirect(Axis::bin_t x, data_t weight)
{
    entries += 1;
    data[xaxis.FindBin( x )] += weight;
}

// ########################################################################

#ifdef H1D_USE_BUFFER
void Histogram1D::FlushBuffer()
{
    if( !buffer.empty() ) {
        for(buffer_t::const_iterator it=buffer.begin(); it<buffer.end(); ++it)
            FillDirect(it->x, it->w);
        buffer.clear();
    }
}
#endif /* H1D_USE_BUFFER */

// ########################################################################

void Histogram1D::Reset()
{
#ifdef H1D_USE_BUFFER
    buffer.clear();
#endif /* H1D_USE_BUFFER */
    for(int i=0; i<xaxis.GetBinCountAll(); ++i)
        data[i] = 0;
    entries = 0;
}

