#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <cassert>
#include <filesystem>
#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;
namespace fs = std::filesystem;

// An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// DONE: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  if (fs::exists(kProcDirectory)) {
    for (const auto& entry : fs::directory_iterator(kProcDirectory)) {
      if (entry.is_directory()) {
        string filename = entry.path().filename().string();
        if (std::all_of(filename.begin(), filename.end(), isdigit)) {
          int pid = stoi(filename);
          pids.push_back(pid);
        }
      }
    }
  }

  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  std::ifstream filestream(kMeminfoFilename);

  std::string line;
  std::string keyword, value, kb;
  std::vector<int> meminfo;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> keyword >> value >> kb;
      assert(kb == "kB");
      meminfo.push_back(std::stoi(value));
      if (meminfo.size() == 4) break;
    }
  }
  if (meminfo.size() == 4) {
    // (MemTotal - MemFree - Buffers) / MemTotal
    return static_cast<float>(meminfo[0] - meminfo[1] - meminfo[3]) /
           static_cast<float>(meminfo[0]);
  }
  return 0.0;
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  std::ifstream filestream(kUptimeFilename);

  std::string line;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      float uptime;
      linestream >> uptime;
      return static_cast<long>(uptime);
    }
  }
  return 0;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid [[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// Read and return CPU utilization
unsigned long long prevUser = 0, prevNice = 0, prevSystem = 0, prevIdle = 0,
                   prevIoWait = 0, prevIrq = 0, prevSoftIrq = 0, prevSteal = 0;

vector<string> LinuxParser::CpuUtilization() {
  std::ifstream filestream(kProcDirectory + kStatFilename);
  std::string line;
  std::getline(filestream, line);
  std::istringstream linestream(line);
  uint64_t user, nice, system, idle, iowait, irq, softirq, steal, guest,
      guest_nice;
  linestream >> user >> nice >> system >> idle >> iowait >> irq >> softirq >>
      steal;
  // Calculate CPU's utilization

  uint64_t PrevIdle = prevIdle + prevIoWait;
  uint64_t Idle = idle + iowait;

  uint64_t PrevNonIdle =
      prevUser + prevNice + prevSystem + prevIrq + prevSoftIrq + prevSteal;
  uint64_t NonIdle = user + nice + system + irq + softirq + steal;

  uint64_t PrevTotal = PrevIdle + PrevNonIdle;
  uint64_t Total = Idle + NonIdle;

  // Calculate differences
  uint64_t totald = Total - PrevTotal;
  uint64_t idled = Idle - PrevIdle;

  // Calculate CPU usage percentage
  auto CPU_Percentage = static_cast<float>(totald - idled) / totald * 100.0;

  // Update previous values for next calculation
  prevUser = user;
  prevNice = nice;
  prevSystem = system;
  prevIdle = idle;
  prevIoWait = iowait;
  prevIrq = irq;
  prevSoftIrq = softirq;
  prevSteal = steal;

  return {std::to_string(CPU_Percentage)};
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  std::ifstream filestream(kProcDirectory + kStatFilename);
  std::string line;
  while (std::getline(filestream, line)) {
    // line should start with "processes"
    if (line.find("processes") != std::string::npos) {
      std::istringstream linestream(line);
      std::string keyword;
      int processes;
      linestream >> keyword >> processes;
      return processes;
    }
  }
  return 0;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::ifstream filestream(kProcDirectory + kStatFilename);
  std::string line;
  while (std::getline(filestream, line)) {
    // line should start with "processes"
    if (line.find("procs_running") != std::string::npos) {
      std::istringstream linestream(line);
      std::string keyword;
      int processes;
      linestream >> keyword >> processes;
      return processes;
    }
  }
  return 0;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kCmdlineFilename);
  std::string line;
  std::getline(filestream, line);
  return line;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatFilename);
  std::string line;
  while (std::getline(filestream, line)) {
    if (line.find("VmSize") != std::string::npos) {
      std::istringstream linestream(line);
      std::string keyword;
      int ram;
      linestream >> keyword >> ram;
      return std::to_string(ram / 1024) + " MB";
    }
  }
  return "";
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);
  std::string line;
  while (std::getline(filestream, line)) {
    if (line.find("Uid") != std::string::npos) {
      std::istringstream linestream(line);
      std::string keyword, uid;
      linestream >> keyword >> uid;
      return uid;
    }
  }
  return "";
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  std::ifstream filestream(kPasswordPath);
  std::string line;
  auto id = Uid(pid);
  while (std::getline(filestream, line)) {
    std::istringstream linestream(line);
    std::string username, x, uid;
    std::replace(line.begin(), line.end(), ':', ' ');
    linestream >> username >> x >> uid;
    if (uid == Uid(pid)) {
      return username;
    }
  }

  return "";
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatFilename);
  std::string line;
  std::getline(filestream, line);
  std::istringstream linestream(line);
  std::string value;
  for (int i = 0; i < 22; i++) {
    linestream >> value;
  }
  // clock ticks to seconds
  return std::stol(value) / sysconf(_SC_CLK_TCK);
}

// Read and return the CPU utilization of a process
float LinuxParser::CpuUtilization(int pid) {
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatFilename);
  std::string line;
  std::getline(filestream, line);
  std::istringstream linestream(line);
  std::string value;
  float uptime, utime, stime, cutime, cstime, starttime;

  linestream >> uptime;
  for (int i = 0; i < 12; i++) {
    linestream >> value;
  }
  linestream >> utime >> stime >> cutime >> cstime;
  for (int i = 0; i < 4; i++) {
    linestream >> value;
  }
  linestream >> starttime;
  auto total_time = utime + stime;
  total_time = total_time + cutime + cstime;
  auto seconds = uptime - (starttime / sysconf(_SC_CLK_TCK));
  auto cpu_usage = 100 * ((total_time / sysconf(_SC_CLK_TCK)) / seconds);
}
