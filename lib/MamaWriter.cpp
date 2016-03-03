
#include "MamaWriter.h"

#include "Histogram1D.h"
#include "Histogram2D.h"
#include "IOPrintf.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <time.h>

#define NDEBUG
#include "debug.h"

//! Encode the current date and time like '22-Mar-94 HH:MM:SS'
/*! \return a string with the date as text
 */
static std::string datetime()
{
    char tmp[64];
    time_t now = time(0);
    int s = strftime(tmp, sizeof(tmp), "%d-%b-%y %H:%M:%S", localtime(&now));
    tmp[s] = '\0';
    return tmp;
}

// ########################################################################

//! Write spectrum or matrix header.
/*! \param fp       output file, must be open already
 *  \param comment  comment for spectrum/matrix
 *  \param xdim     x dimension for spectrum/matrix
 *  \param ydim     <0 for spectrum, y dimension for matrix
 *  \param cal      calibration coefficients
 */
static void spectrum_write_header(std::ostream& fp, const std::string& comment,
                                  int xdim, int ydim, float *cal)
{
    if(cal[0] + cal[1] + cal[2] == 0) {
        cal[0] = 0.;
        cal[1] = 1.;
        cal[2] = 0.;
    }
    if(ydim<0 && cal[3] + cal[4] + cal[5] == 0) {
        cal[3] = 0.;
        cal[4] = 1.;
        cal[5] = 0.;
    }

    const int ical = (ydim<0) ? 3 : 6;

    fp << "!FILE=Disk \n"
       << "!KIND=" << ( (ydim<0) ? "Spectrum" : "Matrix" ) << " \n"
       << "!LABORATORY=Oslo Cyclotron Laboratory (OCL) \n"
       << "!EXPERIMENT=Sirius \n"
       << "!COMMENT=" << comment << '\n'
       << "!TIME=" << datetime() << '\n'
       << "!CALIBRATION EkeV=" << ical;
    for(int i=0; i<ical; i++)
        fp << ioprintf(",%13.6E", cal[i]);
    fp << "\n"
       << "!PRECISION=16\n";
    if( ydim<0 ) {
        fp << "!DIMENSION=1,0:" << ioprintf("%4d", xdim-1) << '\n'
           << "!CHANNEL=(0:" << ioprintf("%4d", xdim-1) << ")\n";
    } else {
        fp << ioprintf("!DIMENSION=2,0:%4d,0:%4d\n", xdim-1, ydim-1)
           << ioprintf("!CHANNEL=(0:%4d,0:%4d)\n",   xdim-1, ydim-1);
    }
}

// ########################################################################

int MamaWriter::Write(std::ofstream& fp, Histogram1D* h)
{
    const Axis& xax = h->GetAxisX();
    float cal[3] = { xax.GetLeft(), xax.GetBinWidth(), 0 };
    spectrum_write_header(fp, h->GetTitle(), xax.GetBinCount(), -1, cal);
    for(int i=0; i<xax.GetBinCount(); i++)
        fp << h->GetBinContent(i+1) << ' ';
    fp << "\n!IDEND=\n\n" << std::flush;

    return ( !fp ) ? -1 : 0;
}

// ########################################################################

int MamaWriter::Write(std::ofstream& fp, Histogram2D* h)
{
    const Axis& xax = h->GetAxisX(), yax = h->GetAxisY();
    float cal[6] = {
        xax.GetLeft(), xax.GetBinWidth(), 0,
        yax.GetLeft(), yax.GetBinWidth(), 0
    };
    spectrum_write_header(fp, h->GetTitle(), xax.GetBinCount(), yax.GetBinCount(), cal);
    for(int j=0; j<yax.GetBinCount(); j++) {
        for(int i=0; i<xax.GetBinCount(); i++)
            fp << h->GetBinContent(i+1, j+1) << ' ';
        fp << '\n';
    }
    fp << "!IDEND=\n\n" << std::flush;
    
    return 0;
}
