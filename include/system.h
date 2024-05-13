#ifndef SYSTEM_H
#define SYSTEM_H

#include <atomic>
#include <chrono>
#include <string>
#include <vector>
#include <thread>

#include "process.h"
#include "processor.h"

class System {
 public:
  explicit System(float updateInterval = 1.0f);
  ~System();

  Processor& Cpu();
  std::vector<Process>& Processes();
  [[nodiscard]] float MemoryUtilization() const;
  [[nodiscard]] long UpTime() const;
  [[nodiscard]] int TotalProcesses() const;
  [[nodiscard]] int RunningProcesses() const;
  [[nodiscard]] std::string Kernel() const;
  [[nodiscard]] std::string OperatingSystem() const;

 private:
  void FetchData();
  bool needsUpdate();

  bool isRunning_;
  std::mutex dataMutex_;

  using TimePointTy = std::chrono::time_point<std::chrono::system_clock>;
  float updateInterval_;
  std::thread updateThread_;
  TimePointTy previousTime_;

  Processor cpu_ = {};
  std::vector<Process> processes_ = {};
  std::atomic<float> memoryUtilization_;
  std::atomic<long> uptime_;
  std::atomic<int> totalProcesses_;
  std::atomic<int> runningProcesses_;
  std::string kernel_;
  std::string operatingSystem_;
};

#endif