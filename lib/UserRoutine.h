/* -*- c++ -*-
 *
 *  Created on: 10.03.2010
 *      Author: Alexander Bürger
 */

#ifndef USERROUTINE_H_
#define USERROUTINE_H_

#include "Histograms.h"
#include "Parameters.h"
#include <string>

class Event;

//! Interface for sorting routines.
class UserRoutine {
public:
    //! Virtual no-op destructor.
    virtual ~UserRoutine() { }

    //! Called before starting any sorting.
    virtual bool Start();

    //! Called for a command.
    virtual bool Command(const std::string& command /*!< The command text to process. */);

    //! Called to sort an event.
    virtual bool Sort(const Event& event /*!< The event structure filled with data. */) = 0;

    //! Called after all sorting is finished.
    virtual bool End();

    Parameters& GetParameters()
        { return parameters; }

    Histograms& GetHistograms()
        { return histograms; }

private:
    //! The list of parameters.
    Parameters parameters;

    //! The list of histograms.
    Histograms histograms;
};

#endif /* USERROUTINE_H_ */
