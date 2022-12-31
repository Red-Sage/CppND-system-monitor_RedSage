#include <string>
#include <vector>
#include "processor.h"
#include "linux_parser.h"
#include <iostream>

using std::vector;
using std::string;

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {

  vector<string> cpu_utilization = LinuxParser::CpuUtilization();
  long total = LinuxParser::Jiffies();
  long active = LinuxParser::ActiveJiffies();

  // Calculate change since last call
  long delta_total = total - previous_total;
  long delta_active = active - previous_active;

  previous_total = total;
  previous_active = active;

  float utilization = (float)delta_active/(float)delta_total;
  
  return utilization;
}