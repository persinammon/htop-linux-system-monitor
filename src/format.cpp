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
  string formattedTime{""};
  int minutes = floor(seconds / 60); // gives time in double minutes, removing extra seconds
  int finalSeconds = seconds - minutes*60; // finds the extra seconds that were removed
  int hours = floor(minutes / 60); // changes the number of minutes to hours, and gets rid of extra minutes
  minutes = minutes - hours*60; // finds the extra minutes
  formattedTime += (hours < 10) ? ("0" + to_string(hours) + ":") : (to_string(hours) + ":");
  formattedTime += (minutes < 10) ? ("0" + to_string(minutes) + ":") : (to_string(minutes) + ":");
  formattedTime += (finalSeconds < 10) ? ("0" + to_string(finalSeconds)) : (to_string(finalSeconds));
  return formattedTime; 
}
