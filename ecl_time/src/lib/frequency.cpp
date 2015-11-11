/**
 * @file /ecl_time/src/lib/frequency.cpp
 */
/*****************************************************************************
** Includes
*****************************************************************************/

#include "../../include/ecl/time/frequency_diagnostics.hpp"

/*****************************************************************************
** Namespaces
*****************************************************************************/

namespace ecl {

/*****************************************************************************
** Implementation
*****************************************************************************/

FrequencyMonitor::FrequencyMonitor(
    const float& window_period,
    const bool& use_realtime_clock
)
: current_diagnostics()
, incoming_counter(0)
, last_incoming()
, last_anyalsed()
, minimum_interval(100000.0) // something ridiculously large so the next interval will fall underneath it
, maximum_interval(0.0)
, period(window_period)
, use_realtime_clock(use_realtime_clock)
{
}

void FrequencyMonitor::update() {
  current_diagnostics.has_connection = true;
  incoming_counter++;
  ecl::TimeStamp new_incoming = (use_realtime_clock) ? ecl::TimeStamp::realtime_now() : ecl::TimeStamp();
  ecl::TimeStamp time_since_last = new_incoming - last_incoming;
  if ( time_since_last < minimum_interval ) {
    minimum_interval = time_since_last;
  }
  if ( time_since_last > maximum_interval ) {
    maximum_interval = time_since_last;
  }
  last_incoming = new_incoming;
}

const FrequencyDiagnostics& FrequencyMonitor::analyse() {
  ecl::TimeStamp now = ecl::TimeStamp::realtime_now();
  ecl::TimeStamp interval = now - last_anyalsed;

  /// every two seconds, we update the status as follows
  if( interval > period )
  {
    last_anyalsed = now;
    if (incoming_counter == 0 ) {
      current_diagnostics.has_connection = false;
      current_diagnostics.minimum_interval = ecl::TimeStamp(0.0);
      current_diagnostics.maximum_interval = ecl::TimeStamp(0.0);
      current_diagnostics.hz = 0.0;
    } else {
      current_diagnostics.has_connection = true;
      current_diagnostics.minimum_interval = minimum_interval;
      current_diagnostics.maximum_interval = maximum_interval;
      current_diagnostics.hz = static_cast<float>(incoming_counter) / interval;

      // clear temporary variables
      incoming_counter = 0;
      minimum_interval = ecl::TimeStamp(100000.0); // something ridiculously large so the next interval will fall underneath it
      maximum_interval = ecl::TimeStamp(0.0);
    }
  }
  return current_diagnostics;
}

/*****************************************************************************
 ** Trailers
 *****************************************************************************/

} // namespace ecl
