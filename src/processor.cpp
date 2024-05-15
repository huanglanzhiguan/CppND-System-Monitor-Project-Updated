#include "processor.h"

Processor::Processor() {
  auto ret = LinuxParser::CpuUtilization();
  prev_user = ret[0];
  prev_nice = ret[1];
  prev_system = ret[2];
  prev_idle = ret[3];
  prev_iowait = ret[4];
  prev_irq = ret[5];
  prev_softirq = ret[6];
  prev_steal = ret[7];
}

// Return the aggregate CPU utilization
float Processor::Utilization() {
  auto ret = LinuxParser::CpuUtilization();
  uint64_t curr_user = ret[0];
  uint64_t curr_nice = ret[1];
  uint64_t curr_system = ret[2];
  uint64_t curr_idle = ret[3];
  uint64_t curr_iowait = ret[4];
  uint64_t curr_irq = ret[5];
  uint64_t curr_softirq = ret[6];
  uint64_t curr_steal = ret[7];

  // Calculate previous and current total idle times
  uint64_t prev_total_idle = prev_idle + prev_iowait;
  uint64_t curr_total_idle = curr_idle + curr_iowait;

  // Calculate previous and current total non-idle times
  uint64_t prev_total_non_idle = prev_user + prev_nice + prev_system +
                                 prev_irq + prev_softirq + prev_steal;
  uint64_t curr_total_non_idle = curr_user + curr_nice + curr_system +
                                 curr_irq + curr_softirq + curr_steal;

  // Calculate previous and current total times
  uint64_t prev_total = prev_total_idle + prev_total_non_idle;
  uint64_t curr_total = curr_total_idle + curr_total_non_idle;

  // Calculate the differences
  uint64_t total_diff = curr_total - prev_total;
  uint64_t idle_diff = curr_total_idle - prev_total_idle;

  // Calculate CPU percentage
  auto diff = static_cast<float>(total_diff) - static_cast<float>(idle_diff);
  float cpu_utilization = std::min(100.0f, diff / static_cast<float>(total_diff));

  prev_user = curr_user;
  prev_nice = curr_nice;
  prev_system = curr_system;
  prev_idle = curr_idle;
  prev_iowait = curr_iowait;
  prev_irq = curr_irq;
  prev_softirq = curr_softirq;
  prev_steal = curr_steal;

  return cpu_utilization;
}