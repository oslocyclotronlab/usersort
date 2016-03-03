// -*- c++ -*-

#ifndef RootWriter_H_
#define RootWriter_H_ 1

#include <string>

class TH1;
class TH2;

typedef TH1* TH1p;
typedef TH2* TH2p;

class Histogram1D;
class Histogram2D;

class Histograms;

//! Functions to write histograms into ROOT files.
class RootWriter {
public:
    //! Write many histograms at once.
    /*! All of the histograms in the list will be written. The output
     *  file will be overwritten if it exists.
     */
    static void Write( Histograms& histograms,     /*!< The histogram list. */
                       const std::string& filename /*!< The output filename. */);

    //! Create a ROOT histogram from a Histogram1D.
    /*! \return the ROOT 1D histogram.
     */
    static TH1p CreateTH1(Histogram1D* h /*!< The Histogram1D to be cpoied. */);

    //! Create a ROOT histogram from a Histogram2D.
    /*! \return the ROOT 2D histogram.
     */
    static TH2p CreateTH2(Histogram2D* h /*!< The Histogram2D to be cpoied. */);
};

#endif /* RootWriter_H_ */
