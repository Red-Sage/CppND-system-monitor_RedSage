#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <unistd.h>
#include "linux_parser.h"
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  int Pid();                               // TODO: See src/process.cpp
  std::string User();                      // TODO: See src/process.cpp
  std::string Command();                   // TODO: See src/process.cpp
  float CpuUtilization();                  // TODO: See src/process.cpp
  std::string Ram();                       // TODO: See src/process.cpp
  long int UpTime();                       // TODO: See src/process.cpp
  bool operator<(Process const& a) const;  // TODO: See src/process.cpp
  void Update();
  Process(int pid) : pid_{pid},
                     user_{LinuxParser::User(pid)},
                     clock_tick_{sysconf(_SC_CLK_TCK)},
                     cmd_{LinuxParser::Command(pid)},
                     previous_total_{LinuxParser::ActiveJiffies(pid)},
                     previous_active_{LinuxParser::Jiffies()}
                     {}

  // TODO: Declare any necessary private members
 private:
  int pid_;
  std::string user_;
  long clock_tick_;
  std::string cmd_;
  long previous_total_;
  long previous_active_;
  float utilization_ = 0;
  float CalcCpuUtil();
  std::string vm_size_;
  long up_time_=0;
  
  
};

#endif