// -*- c++ -*-

#ifndef RATEMETER_H
#define RATEMETER_H 1

#include <sys/time.h>

//! Class fore measuring rates.
/*! This class allows to measure rates of events, buffers, ... After
 *  each processing, Rate() should be called. Unless the intervals are
 *  fixed, this should return a positive value at intervals of about
 *  one second. The return value is the rate with which Rate() had
 *  been called since the last time it returned a positive value.
 *
 *  Example:
 *  <code><pre>
 *  RateMeter rm(1000, false);
 *  for(int i=0; i<1000000; ++i) {
 *      // do something
 *      const float rate = rm.Rate();
 *      if( rate>0 ) {
 *          // show rate (roughly every second)
 *      }
 *  }</pre></code>
 */
class RateMeter {
public:
    //! Construct a ratemeter.
    /*! The intervals at which Rate() returns a positive value (a
     *  rate) can be fixed. If not fixed, the intervals are calculated
     *  to be about one second.
     */
    RateMeter(int default_delta, /*!< the initial step for returning a rate */
              bool fixed         /*!< whether the steps should be fixed or 1/s */);

    //! Reset the ratemeter as if it were new.
    void Reset();

    //! Count and sometimes return a rate.
    /*! At some intervals, the rate of calling this function is
     *  returned. If the intervals are not fixed, they are updated to
     *  have about 1/s.
     * 
     *  \return a rate (positive) or -1 if the interval has not been
     *  reached
     */
    inline float Rate()
        { if( ++count < printit ) return -1; else return RateCalculation(); }


    //! Return the total rate since construction / Reset().
    /*! \return a rate (positive) or -1 if the interval has not been
     *  reached
     */
    float TotalRate();

private:
    //! The default interval between two rate returns in Rate().
    const int default_delta;

    //! Whether the intervals are fixed or updated dynamically.
    const bool fixed;

    //! The number of times Rate() has been called.
    int count;

    //! The size of the present interval.
    int delta;

    //! The next count at which a rate is to be calculated.
    int printit;

    //! The time of the construction / last Reset().
    timeval start;

    //! The time of the last rate calculation.
    timeval last;

    //! Calculate a time difference between old and now.
    /*! The old time is updated.
     *
     *  \return the time difference
     */
    float Timediff(timeval& old /*!< old time value to compare with*/);

    //! Perform the actual rate calculation.
    /*! \return a rate, always positive
     */
    float RateCalculation();

};

#endif /* RATEMETER_H */
