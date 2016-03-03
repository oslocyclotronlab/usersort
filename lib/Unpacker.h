/* -*- c++ -*-
 * Unpacker.h
 *
 *  Created on: 10.03.2010
 *      Author: Alexander Bürger
 */

#ifndef UNPACKER_H_
#define UNPACKER_H_

class Buffer;
class Event;

//! Unpacks data from a Buffer into an Event.
class Unpacker {
public:
    typedef enum { OKAY, //!< Event unpacked without problems.
        END,             //!< End of buffer reached, no event unpacked.
        ERROR            //!< Error while unpacking event.
    } Status;

    //! Initialize the unpacker.
    Unpacker();

    //! Set the buffer from which the events shall be extracted.
    /*! This also resets all counters.
     */
    void SetBuffer(const Buffer* buffer /*!< The buffer to extract from. */);

    //! Unpack the next event.
    /*! \return the status after unpacking
     */
    Status Next(Event& event /*!< The event structure to unpack into. */);

    //! Retrieve the average event length seen so far.
    /*! \return The average event length.
     */
    float GetAverageLength() const
        { return event_count>0 ? eventlength_sum/float(event_count) : 0; }

private:

    //! Actual implementation of the unpacking.
    /*! This method needs to be changed if the box ids change or the
     *  buffer format changes or the Event structure changes.
     */
    bool UnpackOneEvent(unsigned int n_data /*!< The size of this event. */,
                        Event& event /*!< The event structure to fill. */);

    //! The buffer to read from.
    const Buffer* buffer;

    //! The current reading position in the buffer.
    unsigned int buffer_idx;

    //! Sum of event lengths so far.
    int eventlength_sum;

    //! Number of events so far.
    int event_count;
};

#endif /* UNPACKER_H_ */
