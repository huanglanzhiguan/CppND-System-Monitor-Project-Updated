#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <cassert>
#include <experimental/filesystem>
#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;
namespace fs = std::experimental::filesystem;

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
      if (entry.status().type() == fs::file_type::directory) {
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
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);

  std::string line;
  std::string keyword, value, kb;
  std::vector<int> meminfo;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> keyword >> value >> kb;
      assert(kb == "kB");
      meminfo.push_back(std::stoi(value));
      if (meminfo.size() == 5) break;
    }
  }
  if (meminfo.size() == 5) {
    auto totalMemory = static_cast<float>(meminfo[0]);
    auto freeMemory = static_cast<float>(meminfo[1]);
    auto bufferMemory = static_cast<float>(meminfo[3]);
    auto cacheMemory = static_cast<float>(meminfo[4]);

    auto usedMemory = totalMemory - freeMemory - bufferMemory - cacheMemory;
    return usedMemory / totalMemory;
  }

  return 0.0;
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  std::ifstream filestream(kProcDirectory + kUptimeFilename);

  std::string line;
  if (filestream.is_open()) {
    while (std::getline(filestream, line, '.')) {
      std::istringstream linestream(line);
      long uptime;
      linestream >> uptime;
      return uptime;
    }
  }
  return 0;
}

// Read and return CPU utilization
vector<uint64_t> LinuxParser::CpuUtilization() {
  std::ifstream filestream(kProcDirectory + kStatFilename);
  std::string line;
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    std::string cpu;
    uint64_t curr_user, curr_nice, curr_system, curr_idle, curr_iowait,
        curr_irq, curr_softirq, curr_steal;

    linestream >> cpu >> curr_user >> curr_nice >> curr_system >> curr_idle >>
        curr_iowait >> curr_irq >> curr_softirq >> curr_steal;

    return {curr_user,   curr_nice, curr_system,  curr_idle,
            curr_iowait, curr_irq,  curr_softirq, curr_steal};
  }
  return {};
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  std::ifstream filestream(kProcDirectory + kStatFilename);
  std::string line;
  if (filestream.is_open()) {
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
  }
  return 0;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::ifstream filestream(kProcDirectory + kStatFilename);
  std::string line;
  if (filestream.is_open()) {
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
  }
  return 0;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kCmdlineFilename);
  std::string line;
  if (filestream.is_open()) {
    std::getline(filestream, line);
  }
  return line;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);
  std::string line;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (line.find("VmSize") != std::string::npos) {
        std::istringstream linestream(line);
        std::string keyword;
        long ram;
        linestream >> keyword >> ram;
        return std::to_string(ram / 1024);
      }
    }
  }
  return "0";
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);
  std::string line;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (line.find("Uid") != std::string::npos) {
        std::istringstream linestream(line);
        std::string keyword, uid;
        linestream >> keyword >> uid;
        return uid;
      }
    }
  }
  return "";
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  std::ifstream filestream(kPasswordPath);
  std::string line;
  auto id = Uid(pid);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      std::string username, x, uid;
      linestream >> username >> x >> uid;
      if (uid == id) {
        return username;
      }
    }
  }

  return "";
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatFilename);
  std::string line;
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    std::string value;
    for (int i = 0; i < 22; i++) {
      linestream >> value;
    }
    // clock ticks to seconds
    return UpTime() - std::stol(value) / sysconf(_SC_CLK_TCK);
  }
  return 0;
}

// Read and return the CPU utilization of a process
float LinuxParser::CpuUtilization(int pid) {
  auto uptime = static_cast<float>(UpTime());
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatFilename);
  std::string line;
  float utime, stime, cutime, cstime, starttime;

  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    std::string value;

    for (int i = 0; i < 13; i++) {
      linestream >> value;
    }
    linestream >> utime >> stime >> cutime >> cstime;
    for (int i = 0; i < 4; i++) {
      linestream >> value;
    }
    linestream >> starttime;

    auto Hertz = static_cast<float>(sysconf(_SC_CLK_TCK));

    // Calculate the total time spent by the process
    float total_time = utime + stime + cutime + cstime;

    // Calculate the seconds the process has been alive
    float seconds = uptime - (starttime / Hertz);

    // Calculate CPU utilization
    float cpu_utilization = (total_time / Hertz) / seconds;

    return cpu_utilization;
  }
  return 0;
}
