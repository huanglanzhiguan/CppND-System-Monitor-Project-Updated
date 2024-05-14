#include "process.h"

#include "linux_parser.h"

using std::string;

Process::Process(int pid)
    : cpuUtilization_(LinuxParser::CpuUtilization(pid)), pid_(pid) {}

// Return this process's ID
int Process::Pid() const { return pid_; }

// Return this process's CPU utilization
float Process::CpuUtilization() const {
  return cpuUtilization_;
}

// Return the command that generated this process
string Process::Command() const { return LinuxParser::Command(pid_); }

// Return this process's memory utilization
string Process::Ram() const { return LinuxParser::Ram(pid_); }

// Return the user (name) that generated this process
string Process::User() const { return LinuxParser::User(pid_); }

// Return the age of this process (in seconds)
long int Process::UpTime() const { return LinuxParser::UpTime(pid_); }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
  return cpuUtilization_ < a.cpuUtilization_;
}