#ifndef SYSTEM_H
#define SYSTEM_H

#include <atomic>
#include <chrono>
#include <mutex>
#include <string>
#include <vector>
#include <thread>

#include "process.h"
#include "processor.h"

class System {
 public:
  System();
  ~System() = default;

  Processor& Cpu();
  std::vector<Process>& Processes();
  [[nodiscard]] static float MemoryUtilization();
  [[nodiscard]] static long UpTime();
  [[nodiscard]] static int TotalProcesses();
  [[nodiscard]] static int RunningProcesses();
  [[nodiscard]] std::string Kernel() const;
  [[nodiscard]] std::string OperatingSystem() const;

 private:
  std::mutex dataMutex_;

  Processor cpu_ = {};
  std::vector<Process> processes_ = {};
  const std::string kernel_;
  const std::string operatingSystem_;
};

#endif
