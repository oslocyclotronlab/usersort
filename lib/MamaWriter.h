// -*- c++ -*-

#ifndef MamaWriter_H_
#define MamaWriter_H_ 1

#include <iosfwd>

class Histogram1D;
class Histogram2D;

//! Class for writing histograms in MAMA format.
class MamaWriter {
public:

    //! Write a single 1D histogram in MAMA format.
    /*! \return 0 if okay, <0 if error
     */
    static int Write(std::ofstream& out, /*!< The output stream to write to. */
                     Histogram1D* h      /*!< The histogram to write. */);

    //! Write a single 2D histogram in MAMA format.
    /*! \return 0 if okay, <0 if error
     */
    static int Write(std::ofstream& out, /*!< The output stream to write to. */
                     Histogram2D* h      /*!< The histogram to write. */);

};

#endif /* MamaWriter_H_ */
