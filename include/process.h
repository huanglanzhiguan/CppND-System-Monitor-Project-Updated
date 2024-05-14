#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  explicit Process(int pid);

  [[nodiscard]] int Pid() const;
  [[nodiscard]] std::string User() const;
  [[nodiscard]] std::string Command() const;
  [[nodiscard]] float CpuUtilization() const;
  [[nodiscard]] std::string Ram() const;
  [[nodiscard]] long int UpTime() const;
  bool operator<(Process const& a) const;

  float cpuUtilization_;

 private:
  int pid_;
};

#endif