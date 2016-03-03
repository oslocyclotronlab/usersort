/*
 * Event.cpp
 *
 *  Created on: 10.03.2010
 *      Author: Alexander Bürger
 */

#include "Event.h"

void Event::Reset()
{
    n_e = n_de = n_na = n_scaler = 0;
    has_time = has_pattern = false;
}
