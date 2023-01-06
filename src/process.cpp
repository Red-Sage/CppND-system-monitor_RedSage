#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

float Process::CalcCpuUtil(){
  // Private function to simplify calculation of utilization
  long total_jiffies = LinuxParser::Jiffies();
  long active = LinuxParser::ActiveJiffies(pid_);
  

  long delta_active = previous_active_ - active;
  long delta_total = previous_total_ - total_jiffies;

  float util = (float)delta_active/(float)delta_total;

  previous_active_ = active;
  previous_total_ = total_jiffies;
  
  return util;
}

void Process::Update(){
  // The update method is provided to update the things that change
  // over time. As opposed to the things like cmd that can be set once.
  utilization_ = CalcCpuUtil();
  vm_size_ = LinuxParser::Ram(pid_);
  up_time_ = LinuxParser::UpTime() - start_time_;
}

// TODO: Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { return utilization_; }

// TODO: Return the command that generated this process
string Process::Command() { return cmd_; }

// TODO: Return this process's memory utilization
string Process::Ram() { return vm_size_; }

// TODO: Return the user (name) that generated this process
string Process::User() { return user_; }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return up_time_; }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { 
  return this->utilization_ < a.utilization_;
}
