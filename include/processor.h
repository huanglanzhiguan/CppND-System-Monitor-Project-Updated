#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "linux_parser.h"

class Processor {
 public:
  Processor();
  float Utilization();

 private:
  uint64_t prev_user, prev_nice, prev_system, prev_idle, prev_iowait, prev_irq,
      prev_softirq, prev_steal;
};

#endif