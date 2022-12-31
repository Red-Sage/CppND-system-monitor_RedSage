#include <string>
#include <sstream>
#include <iomanip>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  long seconds_in_hour = 60*60;
  long seconds_in_minute = 60;
  std::ostringstream formatted_time;
  //formatted_time.precision(10);
  // string formatted_time = "";
  
  long h = seconds/seconds_in_hour;
  long m = (seconds-h*seconds_in_hour)/seconds_in_minute;
  long s = (seconds-h*seconds_in_hour-m*seconds_in_minute);

  formatted_time << std::setw(2) << std::setfill('0') << h << ":" << std::setw(2) << std::setfill('0') << m << ":" << std::setw(2) << std::setfill('0') << s;
  return formatted_time.str();
}

