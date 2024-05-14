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

System::System()
    : cpu_(Processor()),
      kernel_(LinuxParser::Kernel()),
      operatingSystem_(LinuxParser::OperatingSystem()) {
}

// Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// Return a container composed of the system's processes
vector<Process>& System::Processes() {
  processes_.clear();
  std::vector<int> pids = LinuxParser::Pids();

  for (int pid : pids) {
    processes_.emplace_back(pid);
  }

  std::sort(processes_.rbegin(), processes_.rend());
  return processes_;
}

// Return the system's kernel identifier (string)
std::string System::Kernel() const { return kernel_; }

// Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// Return the operating system name
std::string System::OperatingSystem() const { return operatingSystem_; }

// Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }