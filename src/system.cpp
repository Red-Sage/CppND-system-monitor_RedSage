#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;
/*You need to complete the mentioned TODOs in order to satisfy the rubric criteria "The student will be able to extract and display basic data about the system."

You need to properly format the uptime. Refer to the comments mentioned in format. cpp for formatting the uptime.*/

System::System() : operating_system_{LinuxParser::OperatingSystem()},
                   kernel_{LinuxParser::Kernel()} {}

// TODO: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() {
  
  
  vector<int> pids = LinuxParser::Pids();
  
  // loop over the existing processes. If they are still running remove them from pids
  // If they don't remove them from the list.
  for (auto it = processes_.begin(); it != processes_.end();){
    
    auto exists_at = std::find(pids.begin(), pids.end(), it->Pid());
    if(exists_at != pids.end()){
      pids.erase(exists_at);
      it->Update();
      ++it;
    }
    else{
      processes_.erase(it);

    }
  }

  // Loop over what is left in pids, these are new and need to be created.
  for (auto it = pids.begin(); it != pids.end(); ++it){
    Process p(*it);
    p.Update();
    processes_.push_back(p);
  }
  
  std::sort(processes_.begin(), processes_.end());
  std::reverse(processes_.begin(), processes_.end());
  return processes_;
}

// TODO: Return the system's kernel identifier (string)
std::string System::Kernel() { return kernel_; }

// TODO: Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// TODO: Return the operating system name
std::string System::OperatingSystem() { return operating_system_; }

// TODO: Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// TODO: Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// TODO: Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }
