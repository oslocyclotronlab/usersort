/*
 *
 *  Created on: 17.03.2010
 *      Author: Alexander Bürger
 */

#include "ParticleRange.h"

#include <fstream>
#include <iostream>
#include <sstream>

#ifndef TEST_PARTICLERANGE
#define NDEBUG 1
#endif
#include "debug.h"

// ########################################################################

ParticleRange::ParticleRange()
    : Emin( 200 )
    , Estep( 10 )
{
}

// ########################################################################

//! Read one line from a zrange file.
/*! Jumps over comments (# at start) and empty lines. At the end of
 *  the file, energy, range, and uncertainty will be 0.
 */
static void readline(std::istream& in, /*!< The input stream. */
                     float& e,         /*!< The energy read from the stream, converted to keV. */
                     float& r,         /*!< The range at this energy, in um. */
                     float& dr         /*!< The range uncertainty, in um. */)
{
    std::string line;
    while( getline(in, line) ) {
        if( line.empty() )
            continue;
        if( line[0] == '#' )
            continue;

        std::istringstream l( line.c_str() );
        l >> e >> r >> dr; // read energy [MeV], range [um], range uncertainty [um]
        e *= 1000;         // convert to energy keV
        if( !in )          // check for errors
            break;
        return;
    }
    // end of file, or error: everything 0
    e = r = dr = 0;
}

// ########################################################################

void ParticleRange::Read(const std::string & filename)
{
    // forget old interpolation
    values.clear();

    // open zrange file
    std::ifstream f( filename.c_str() );

    float flE, flR, flDR; // last energy, range, drange from file
    readline( f, flE, flR, flDR );
    float fE, fR, fDR;
    readline( f, fE, fR, fDR );

    const float hstep = Estep/2.0;

    for(int E = Emin; !(!f); E += Estep ) {
        while( f && fE<E+hstep ) {
            flE  = fE;
            flR  = fR;
            flDR = fDR;
            readline( f, fE, fR, fDR );
        }
        if( !f || fE == 0 )
            break;
        const float x = (E+hstep-flE)/(fE-flE);
        const float eR = fR*x + flR*(1-x);
        values.push_back( eR );
    }
    std::cout << "ParticleRange: " << values.size() << " interpolation points" << std::endl;
}

// ########################################################################

float ParticleRange::GetRange(int energy) const
{
    if( energy<Emin )
        return 0;
    unsigned int index = (energy-Emin)/Estep;
    if( index >= values.size() )
        return 1e6;
    return values[index];
}

// ########################################################################
// ########################################################################

#ifdef TEST_PARTICLERANGE

#include <iostream>

int main(int argc, char* argv[])
{
    if( argc != 2 )
        return -1;

    ParticleRange range;
    range.Read( argv[1] );

    const int E[] = { 1000, 2000, 3000, 3200, 10000, 30000, 40000, 75000, 79000, 81000, 85000, -1 };
    for( int i=0; E[i]>0; ++i )
        std::cout << "E=" << E[i] << " range=" << range.GetRange( E[i] ) << std::endl;

    return 0;
}

// g++ -Wall -W -O2 -DTEST_PARTICLERANGE -o /tmp/pr.exe ParticleRange.cpp
#endif
