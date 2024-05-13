#include "format.h"

#include <string>

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds [[maybe_unused]]) {
  long hours = seconds / 3600;
  long minutes = (seconds % 3600) / 60;
  long sec = seconds % 60;
  return std::to_string(hours) + ":" + std::to_string(minutes) + ":" +
         std::to_string(sec);
}