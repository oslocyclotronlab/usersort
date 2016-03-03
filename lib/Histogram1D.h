/* -*- c++ -*-
 *
 *  Created on: 10.03.2010
 *      Author: Alexander Bürger
 */

#ifndef HISTOGRAM1D_H_
#define HISTOGRAM1D_H_

#include "Histograms.h"

//#define H1D_USE_BUFFER 1
#ifdef H1D_USE_BUFFER
#include <vector>
#endif

// ########################################################################

//! A one-dimensional histogram.
class Histogram1D : public Named {
public:
    //! The type used to count in each bin.
    typedef double data_t;

    //! Construct a 1D histogram.
    Histogram1D( const std::string& name,  /*!< The name of the new histogram. */
                 const std::string& title, /*!< The title of the new histogram. */
                 int channels,             /*!< The number of regular bins. */
                 Axis::bin_t left,         /*!< The lower edge of the lowest bin.  */
                 Axis::bin_t right,        /*!< The upper edge of the highest bin. */
                 const std::string& xtitle /*!< The title of the x axis. */);

    //! Deallocate memory.
    ~Histogram1D();

    //! Add another histogram.
    void Add(const Histogram1Dp other, data_t scale);

    //! Increment a histogram bin.
    void Fill(Axis::bin_t x,  /*!< The x axis value. */
              data_t weight=1 /*!< How much to add to the corresponding bin content. */)
        {
#ifdef H1D_USE_BUFFER
            buffer.push_back(buf_t(x, weight)); if( buffer.size()>=buffer_max ) FlushBuffer();
#else
            FillDirect(x, weight);
#endif /* H1D_USE_BUFFER */
        }

    //! Get the contents of a bin.
    /*! \return The bin content.
     */
    data_t GetBinContent(int bin /*!< The bin to look at. */);

    //! Set the contents of a bin.
    void SetBinContent(int xbin /*!< The bin to set.  */,
                       data_t c /*!< The bin content. */);

    //! Get the x axis of the histogram.
    /*! \return The histogram's x axis.
     */
    const Axis& GetAxisX() const
        { return xaxis; }

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
                    data_t weight=1 /*!< How much to add to the corresponding bin content. */);

#ifdef H1D_USE_BUFFER
    //! Flush the data buffer.
    void FlushBuffer();
#endif /* H1D_USE_BUFFER */

    //! The x axis of the histogram;
    const Axis xaxis;

    //! The number of entries in the histogram.
    int entries;

    //! The bin contents, including the overflow bins.
    data_t *data;

#ifdef H1D_USE_BUFFER
    struct buf_t {
        Axis::bin_t x;
        data_t w;
        buf_t(Axis::bin_t xx, data_t ww) : x(xx), w(ww) { }
    };
    typedef std::vector<buf_t> buffer_t;
    buffer_t buffer;
    static const unsigned int buffer_max = 1024;
#endif /* H1D_USE_BUFFER */
};

#endif /* HISTOGRAM1D_H_ */
