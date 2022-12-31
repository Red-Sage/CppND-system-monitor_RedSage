#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
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

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  // Per this post https://stackoverflow.com/questions/41224738/how-to-calculate-system-memory-usage-from-proc-meminfo-like-htop/41251290#41251290
  // This is how "Used memory" is calculated:
  // Total used memory = MemTotal - MemFree
  // Used memory = Total used memory - Cached memory
  
  string line;
  string key;
  string value;
  string mem_total;
  string mem_free;
  string cached;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal") {
          mem_total = value;
        }else if(key == "MemFree"){
          mem_free = value;
        }else if(key == "Cached"){
          cached = value;
        }

      }
    }

    return (std::stof(mem_total)-std::stof(mem_free)-std::stof(cached))/std::stof(mem_total);
  }
  return 0.0;
  }

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  string line;
  long up_time = 0.0;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> up_time;
  }
  return up_time;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  
  // I tried this first but it did not seem to work. I created
  // a python script to spin up a bunch of processes and do 
  // randome work. When I ran that code this method showed
  // over 130% utilization. I think this is likely caused by 
  // the time it takes to read differenct data sources. The 
  // implementation below gets all of its data from one file.
  //----------------------------------------------------------
  // long tick_rate = sysconf(_SC_CLK_TCK);
  // long up_time = UpTime();
  // long jiffies = up_time*tick_rate;
  //----------------------------------------------------------
  
  // This works much better
  CPUStates s;
  vector<string> util = CpuUtilization();
  long jiffies = std::stol(util[s=kUser_])
              + std::stol(util[s=kNice_])
              + std::stol(util[s=kSystem_])
              + std::stol(util[s=kIdle_])
              + std::stol(util[s=kIOwait_])
              + std::stol(util[s=kIRQ_])
              + std::stol(util[s=kSoftIRQ_])
              + std::stol(util[s=kSteal_])
              + std::stol(util[s=kGuest_])
              + std::stol(util[s=kGuestNice_]);
  return jiffies;

}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  // From: https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
  // total_time = utime + stime + cutime + cstime (includes child processes)
  string line;
  string utime;
  string stime;
  string cutime;
  string cstime;
  long active = 0;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 13; i++){
      linestream >> utime;
    }
    
    linestream >> utime >> stime >> cutime >> cstime;
    active = std::stol(utime) + std::stol(stime) + std::stol(cutime) + std::stol(cstime);
  }

  
  return active;
  }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  // From this post:  https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux 
  // Active = user + nice + system + irq + softirq + steal + (guest + guestNice) (I added the values in paren)
  CPUStates s;
  vector<string> util = CpuUtilization();
  long active = std::stol(util[s=kUser_])
              + std::stol(util[s=kNice_])
              + std::stol(util[s=kSystem_])
              + std::stol(util[s=kIRQ_])
              + std::stol(util[s=kSoftIRQ_])
              + std::stol(util[s=kSteal_])
              + std::stol(util[s=kGuest_])
              + std::stol(util[s=kGuestNice_]);
  return active;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  // From this post:  https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
  // idle = idle + IOwait
  CPUStates s;
  vector<string> util = CpuUtilization();
  long idle = (std::stol(util[s=kIdle_])
             + std::stol(util[s=kIOwait_]));
  return idle;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  
  string line;
  string key;
  string value;
  vector<string> values;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key == "cpu") {
          while(linestream >> value)
          values.push_back(value);
        }
      }
    }

    return values;
  }
  return values;
  }

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  string key;
  string value = "0";
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          return std::stoi(value);
        }
      }
    }
  }
  return std::stoi(value);  
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  string key;
  string value = "0";
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          return std::stoi(value);
        }
      }
    }
  }
  return std::stoi(value);    
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string line;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if(filestream.is_open()){
    std::getline(filestream, line);
    return line;
  }

  return string();
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string line;
  string key;
  string value;
  float mb_size;
  std::stringstream out_stream;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize") {
          mb_size = std::stof(value);
          mb_size = mb_size/1000000.0; //convert to MB
          out_stream << std::fixed << std::setprecision(2) << mb_size;
          return out_stream.str();
        }
      }
    }
  }
  return string();
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid") {
          return value;
        }
      }
    }
  }
   return string();
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string line;
  string key;
  string value;
  string trash;
  string name;
  string uid = Uid(pid);
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> name >> trash >> key) {
        if (key == uid) {
          return name;
        }
      }
    }
  }
return "Not Found";
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  // Be careful, this passes ticks the user needs to convert to seconds
  string line;
  string start_time;
  long active = 0;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i = 0; i <= 21; i++){
      linestream >> start_time;
    }
  }

  return std::stol(start_time);
}
