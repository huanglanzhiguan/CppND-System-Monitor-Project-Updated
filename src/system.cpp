#include "system.h"

#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

System::System(float updateInterval)
    : isRunning_(true),
      updateInterval_(updateInterval),
      previousTime_(std::chrono::system_clock::now()),
      cpu_(Processor()),
      memoryUtilization_(0),
      uptime_(0),
      totalProcesses_(0),
      runningProcesses_(0),
      kernel_(LinuxParser::Kernel()),
      operatingSystem_(LinuxParser::OperatingSystem()) {
  updateThread_ = std::thread(&System::FetchData, this);
}

System::~System() {
  isRunning_ = false;
  if (updateThread_.joinable()) {
    updateThread_.join();
  }
}

// Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// Return a container composed of the system's processes
vector<Process>& System::Processes() { return processes_; }

// Return the system's kernel identifier (string)
std::string System::Kernel() const { return kernel_; }

// Return the system's memory utilization
float System::MemoryUtilization() const { return memoryUtilization_; }

// Return the operating system name
std::string System::OperatingSystem() const { return operatingSystem_; }

// Return the number of processes actively running on the system
int System::RunningProcesses() const { return runningProcesses_; }

// Return the total number of processes on the system
int System::TotalProcesses() const { return totalProcesses_; }

// Return the number of seconds since the system started running
long int System::UpTime() const { return uptime_; }

void System::FetchData() {
  while (isRunning_ && needsUpdate()) {
    memoryUtilization_ = LinuxParser::MemoryUtilization();
    uptime_ = LinuxParser::UpTime();
    totalProcesses_ = LinuxParser::TotalProcesses();
    runningProcesses_ = LinuxParser::RunningProcesses();

    auto pids = LinuxParser::Pids();

    std::lock_guard<std::mutex> lock(dataMutex_);
    processes_.clear();
    for (auto pid : pids) {
      processes_.emplace_back(pid);
    }
  }
}

bool System::needsUpdate() {
  auto currentTime = std::chrono::system_clock::now();
  if (std::chrono::duration<float>(currentTime - previousTime_).count() >=
      updateInterval_) {
    previousTime_ = currentTime;
    return true;
  }

  return false;
}