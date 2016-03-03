/*
 *  Created on: 10.03.2010
 *      Author: Alexander Bürger
 */

#include "Histogram2D.h"

#include <iostream>

#define NDEBUG
#include "debug.h"

#ifdef H2D_USE_BUFFER
const unsigned int Histogram2D::buffer_max;
#endif /* H2D_USE_BUFFER */

// ########################################################################

Histogram2D::Histogram2D( const std::string& name, const std::string& title,
                          int ch1, Axis::bin_t l1, Axis::bin_t r1, const std::string& xt, 
                          int ch2, Axis::bin_t l2, Axis::bin_t r2, const std::string& yt)
    : Named( name, title )
    , xaxis( name+"_xaxis", ch1, l1, r1, xt )
    , yaxis( name+"_yaxis", ch2, l2, r2, yt )
#ifndef USE_ROWS
    , data( 0 )
#else
    , rows( 0 )
#endif
{
#ifdef H2D_USE_BUFFER
    buffer.reserve(buffer_max);
#endif /* H2D_USE_BUFFER */

#ifndef USE_ROWS
    data = new data_t[xaxis.GetBinCountAll()*yaxis.GetBinCountAll()];
#else
    rows = new data_t*[yaxis.GetBinCountAll()];
    for(int y=0; y<yaxis.GetBinCountAll(); ++y)
        rows[y] = new data_t[xaxis.GetBinCountAll()];
#endif
    Reset();
}

// ########################################################################

Histogram2D::~Histogram2D()
{
#ifndef USE_ROWS
    delete data;
#else
    for(int y=0; y<yaxis.GetBinCountAll(); ++y)
        delete rows[y];
    delete rows;
#endif
}

// ########################################################################

void Histogram2D::Add(const Histogram2Dp other, data_t scale)
{
    if( !other 
        || other->GetName() != GetName()
        || other->GetAxisX().GetLeft() != xaxis.GetLeft()
        || other->GetAxisX().GetRight() != xaxis.GetRight()
        || other->GetAxisX().GetBinCount() != xaxis.GetBinCount()
        || other->GetAxisY().GetLeft() != yaxis.GetLeft()
        || other->GetAxisY().GetRight() != yaxis.GetRight()
        || other->GetAxisY().GetBinCount() != yaxis.GetBinCount() )
        return;

#ifdef H2D_USE_BUFFER
    other->FlushBuffer();
    FlushBuffer();
#endif /* H2D_USE_BUFFER */

#ifndef USE_ROWS
    for(int i=0; i<xaxis.GetBinCountAll()*yaxis.GetBinCountAll(); ++i)
        data[i] += scale * other->data[i];
#else
    for(int y=0; y<yaxis.GetBinCountAll(); ++y )
        for(int x=0; x<xaxis.GetBinCountAll(); ++x )
            rows[y][x] += scale*other->rows[y][x];
#endif
}

// ########################################################################

Histogram2D::data_t Histogram2D::GetBinContent(int xbin, int ybin)
{
#ifdef H2D_USE_BUFFER
    if( !buffer.empty() )
        FlushBuffer();
#endif /* H2D_USE_BUFFER */

    if( xbin>=0 && xbin<xaxis.GetBinCountAll() && ybin>=0 && ybin<yaxis.GetBinCountAll() ) {
#ifndef USE_ROWS
        return data[xaxis.GetBinCountAll()*ybin + xbin];
#else
        return rows[ybin][xbin];
#endif
    } else
        return 0;
}

// ########################################################################

void Histogram2D::SetBinContent(int xbin, int ybin, data_t c)
{
#ifdef H2D_USE_BUFFER
    if( !buffer.empty() )
        FlushBuffer();
#endif /* H2D_USE_BUFFER */

    if( xbin>=0 && xbin<xaxis.GetBinCountAll() && ybin>=0 && ybin<yaxis.GetBinCountAll() ) {
#ifndef USE_ROWS
        data[xaxis.GetBinCountAll()*ybin + xbin] = c;
#else
        rows[ybin][xbin] = c;
#endif
    }
}

// ########################################################################

void Histogram2D::FillDirect(Axis::bin_t x, Axis::bin_t y, data_t weight)
{
    const int xbin = xaxis.FindBin( x );
    const int ybin = yaxis.FindBin( y );
#ifndef USE_ROWS
    data[xaxis.GetBinCountAll()*ybin + xbin] += weight;
#else
    rows[ybin][xbin] += weight;
#endif
    entries += 1;
}

// ########################################################################

#ifdef H2D_USE_BUFFER
void Histogram2D::FlushBuffer()
{
    if( !buffer.empty() ) {
        for(buffer_t::const_iterator it=buffer.begin(); it<buffer.end(); ++it)
            FillDirect(it->x, it->y, it->w);
        buffer.clear();
    }
}
#endif /* H2D_USE_BUFFER */

// ########################################################################

void Histogram2D::Reset()
{
#ifdef H2D_USE_BUFFER
    buffer.clear();
#endif /* H2D_USE_BUFFER */
    for(int y=0; y<yaxis.GetBinCountAll(); ++y )
        for(int x=0; x<xaxis.GetBinCountAll(); ++x )
            SetBinContent( x, y, 0 );
    entries = 0;
}

// ########################################################################
// ########################################################################

#ifdef TEST_HISTOGRAM2D

//#include "RootWriter.h"
//#include <TFile>

int main(int argc, char* argv[])
{
    Histogram2D h("ho", "hohoho", 10,0,10,"xho", 10,0,40, "yho");
    h.Fill( 3,20, 7);
    h.Fill( 4,19, 6);
    h.Fill( 5,-2,1 );
    h.Fill( -1,-1, 10 );

    for(int iy=11; iy>=0; --iy) {
        for(int ix=0; ix<12; ++ix)
            std::cout << h.GetBinContent(ix, iy) << ' ';
        std::cout << std::endl;
    }
}

#endif
