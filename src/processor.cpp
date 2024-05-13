#include "processor.h"

// Return the aggregate CPU utilization
float Processor::Utilization() {
  auto ret = LinuxParser::CpuUtilization();
  return std::stof(ret[0]);
}