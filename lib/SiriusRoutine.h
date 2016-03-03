// -*- c++ -*-

#ifndef SIRIUSROUTINE_H
#define SIRIUSROUTINE_H 1

#include "UserRoutine.h"
#include "Parameters.h"
#include "Histograms.h"

#include <string>

// ########################################################################

//! Class for sorting and writing the spectra for Sirius format.
class SiriusRoutine : public UserRoutine {
public:
    SiriusRoutine();

    bool Start();
    bool Command(const std::string& cmd);
    bool End();

protected:
    //! Obtain the number of seconds since the first timestamp.
    unsigned long Timediff(const Event& event /*!< The event structure maybe containing a new timestamp. */);

    //! Create all spectra.
    /*! This method must be implemented in a class deriving from SiriusRoutine.
     */
    virtual void CreateSpectra() = 0;

    //! Create a 1D histogram.
    Histogram1Dp Spec( const std::string& name,  /*!< The name of the new histogram. */
                       const std::string& title, /*!< The title of teh new histogram. */
                       int channels,             /*!< The number of regular bins. */
                       Axis::bin_t left,         /*!< The lower edge of the lowest bin.  */
                       Axis::bin_t right,        /*!< The upper edge of the highest bin. */
                       const std::string& xtitle /*!< The title of the x axis. */)
        { return GetHistograms().Create1D(name, title, channels, left, right, xtitle ); }

    //! Create a 2D histogram.
    Histogram2Dp Mat( const std::string& name,   /*!< The name of the new histogram. */
                      const std::string& title,  /*!< The title of teh new histogram. */
                      int ch1,                   /*!< The number of regular bins on the x axis. */
                      Axis::bin_t l1,            /*!< The lower edge of the lowest bin on the x axis. */
                      Axis::bin_t r1,            /*!< The upper edge of the highest bin on the x axis. */
                      const std::string& xtitle, /*!< The title of the x axis. */
                      int ch2,                   /*!< The number of regular bins on the y axis. */
                      Axis::bin_t l2,            /*!< The lower edge of the lowest bin on the y axis. */
                      Axis::bin_t r2,            /*!< The upper edge of the highest bin on the y axis. */
                      const std::string& ytitle  /*!< The title of the y axis. */)
        { return GetHistograms().Create2D( name, title, ch1, l1, r1, xtitle, ch2, l2, r2, ytitle ); }

protected:
    //! SiRi E gain parameters
    Parameter  gain_e;

    //! SiRi DE gain parameters
    Parameter  gain_de;

    //! Ge detector gain parameters
    Parameter  gain_ge;

    //! CACTUS E gain parameters
    Parameter  gain_na;

    //! SiRi E shift parameters
    Parameter  shift_e;

    //! SiRi DE shift parameters
    Parameter  shift_de;

    //! Ge detector shift parameters
    Parameter  shift_ge;

    //! CACTUS E shift parameters
    Parameter  shift_na;

    //! Ge time shift parameters
    Parameter shift_tge;

    //! CACTUS time shift parameters
    Parameter shift_tna;

    //! Ge time gain parameters
    Parameter gain_tge;

    //! CACTUS time gain parameters
    Parameter gain_tna;

private:
    //! The first timestamp seen.
    unsigned long time_start;

    //! The time in seconds since the first timestamp.
    unsigned long time_diff;
};

// ########################################################################

#endif /* SIRIUSROUTINE_H */
