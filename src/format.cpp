#include <string>
#include <cmath>

#include "format.h"

using std::string;
using std::to_string;
using std::floor;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
  string formattedTime;
  double long minutes = floor(seconds / 60);
  long finalSeconds = seconds - minutes*60;
  double long hours = floor(minutes / 60);
  minutes = minutes - hours*60;
  return string(to_string(hours) + ":" + to_string(minutes) + ":" + to_string(finalSeconds)); 
}
