/* -*- c++ -*-
 *
 *  Created on: 10.03.2010
 *      Author: Alexander Bürger
 */

#ifndef EVENT_H_
#define EVENT_H_

//! A sirius event.
struct Event {

    //! Combination of channel id and ADC value.
    struct chn_adc {
        //! The channel id.
        int chn;

        //! The ADC value.
        int adc;
    };
    
    //! Combination of channel id, TDC and ADC value.
    struct chn_adc_tdc {
        //! The channel id.
        int chn;

        //! The ADC value.
        int adc;

        //! The TDC value.
        int tdc;
    };
    
    //! Combination of channel id and scaler value.
    struct chn_scaler {
        //! The channel id.
        int chn;

        //! The scaler value.
        long scaler;
    };

    //! The count of SiRi back detector ADC values.
    int n_e;

    //! The SiRi back detector ADC values.
    chn_adc e[32];

    //! The count of SiRi front detector ADC values.
    int n_de;

    //! The SiRi front detetcor ADC values.
    chn_adc de[64];

    //! The count of CACTUS TDC and ADC values.
    int n_na;

    //! The CACTUS TDC and ADC values.
    chn_adc_tdc na[32];

    //! The count of scaler values.
    int n_scaler;

    //! The scaler values.
    chn_scaler scaler[16];

    //! Flag indicating that the wall clock time is present.
    bool has_time;

    //! The wall clock time, if present.
    long time;

    //! Flag indicating that the TPU pattern is present.
    /*! The TPU pattern should always be present. */
    bool has_pattern;

    //! The TPU pattern, if present.
    /*! The TPU pattern should always be present. */
    int pattern;

    //! Reset the event.
    /*! Sets all counters to 0 and all flags to false. */
    void Reset();
};

#endif /* EVENT_H_ */
