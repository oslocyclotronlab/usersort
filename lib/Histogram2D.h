/* -*- c++ -*-
 * Histogram2D.h
 *
 *  Created on: 10.03.2010
 *      Author: Alexander Bürger
 */

#ifndef HISTOGRAM2D_H_
#define HISTOGRAM2D_H_

#include "Histograms.h"

//#define USE_ROWS 1
//#define H2D_USE_BUFFER 1
#ifdef H2D_USE_BUFFER
#include <vector>
#endif

//! A two-dimensional histogram.
class Histogram2D : public Named {
public:
    //! The type used to count in each bin.
    typedef float data_t;

    //! Construct a 2D histogram.
    Histogram2D( const std::string& name,   /*!< The name of the new histogram. */
                 const std::string& title,  /*!< The title of teh new histogram. */
                 int xchannels,             /*!< The number of regular bins on the x axis. */
                 Axis::bin_t xleft,         /*!< The lower edge of the lowest bin on the x axis. */
                 Axis::bin_t xright,        /*!< The upper edge of the highest bin on the x axis. */
                 const std::string& xtitle, /*!< The title of the x axis. */
                 int ychannels,             /*!< The number of regular bins on the y axis. */
                 Axis::bin_t yleft,         /*!< The lower edge of the lowest bin on the y axis. */
                 Axis::bin_t yright,        /*!< The upper edge of the highest bin on the y axis. */
                 const std::string& ytitle  /*!< The title of the y axis. */);

    //! Deallocate memory.
    ~Histogram2D();

    //! Add another histogram.
    void Add(const Histogram2Dp other, data_t scale);

    //! Increment a histogram bin.
    void Fill(Axis::bin_t x,  /*!< The x axis value. */
              Axis::bin_t y,  /*!< The y axis value. */
              data_t weight=1 /*!< How much to add to the corresponding bin content. */)
        {
#ifdef H2D_USE_BUFFER
            buffer.push_back(buf_t(x, y, weight)); if( buffer.size()>=buffer_max ) FlushBuffer();
#else
            FillDirect(x, y, weight);
#endif /* H2D_USE_BUFFER */
        }

    //! Get the contents of a bin.
    /*! \return The bin content.
     */
    data_t GetBinContent(int xbin /*!< The x bin to look at. */,
                         int ybin /*!< The y bin to look at. */);

    //! Set the contents of a bin.
    void SetBinContent(int xbin /*!< The x bin to set. */,
                       int ybin /*!< The y bin to set. */,
                       data_t c /*!< The bin content.  */);

    //! Get the x axis of the histogram.
    /*! \return The histogram's x axis.
     */
    const Axis& GetAxisX() const
        { return xaxis; }

    //! Get the y axis of the histogram.
    /*! \return The histogram's y axis.
     */
    const Axis& GetAxisY() const
        { return yaxis; }

    //! Get the number of entries in the histogram.
    /*! \return The histogram's entry count.
     */
    int GetEntries() const
        { return entries; }

    //! Clear all bins of the histogram.
    void Reset();

private:
    //! Increment a histogram bin directly, bypassing the buffer.
    void FillDirect(Axis::bin_t x,  /*!< The x axis value. */
                    Axis::bin_t y,  /*!< The y axis value. */
                    data_t weight=1 /*!< How much to add to the corresponding bin content. */);

#ifdef H2D_USE_BUFFER
    //! Flush the data buffer.
    void FlushBuffer();
#endif /* H2D_USE_BUFFER */

    //! The x axis of the histogram;
    const Axis xaxis;

    //! The y axis of the histogram;
    const Axis yaxis;

    //! The number of entries in the histogram.
    int entries;

#ifndef USE_ROWS
    //! The bin contents, including the overflow bins.
    data_t *data;
#else
    data_t **rows;
#endif

#ifdef H2D_USE_BUFFER
    struct buf_t {
        Axis::bin_t x, y;
        data_t w;
        buf_t(Axis::bin_t xx, Axis::bin_t yy, data_t ww) : x(xx), y(yy), w(ww) { }
    };
    typedef std::vector<buf_t> buffer_t;
    buffer_t buffer;
    static const unsigned int buffer_max = 4096;
#endif /* H2D_USE_BUFFER */
};

#endif /* HISTOGRAM2D_H_ */
