
#include "RateMeter.h"

#include <algorithm>

RateMeter::RateMeter(int dfl, bool fxd)
    : default_delta( dfl )
    , fixed( fxd )
{
    Reset();
}

// ########################################################################

void RateMeter::Reset()
{
    count = 0;
    delta = default_delta;
    printit = delta;
    gettimeofday(&last, 0);
    start = last;
}

// ########################################################################

float RateMeter::RateCalculation()
{
    const float timediff = Timediff(last);
    const float rate = delta / timediff;
    const float rate_tot = TotalRate();

    delta = fixed ? default_delta : (int)std::max(1.0f, std::min(10000.0f, rate_tot));
    printit += delta;

    return rate;
}

// ########################################################################

float RateMeter::TotalRate()
{
    timeval s = start;
    return count / Timediff( s );
}

// ########################################################################

float RateMeter::Timediff(timeval& old)
{
    timeval now;
    gettimeofday(&now, 0);

    const double o = old.tv_sec + old.tv_usec*1e-6;
    const double n = now.tv_sec + now.tv_usec*1e-6;

    old = now;

    // always return a value > 0, even if it is not the correct time
    // difference
    return std::max( n-o, 1e-12 );
}
