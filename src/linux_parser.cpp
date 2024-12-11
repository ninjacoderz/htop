#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <sstream>
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

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // DT_DIR: file type is directory 
    if (file->d_type == DT_DIR) {
      // Process ID is digit
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
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  string line, key, value;
  float utilization = 0.0;
  float memTotal, memFree;
  
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::replace(line.begin(), line.end(), 'k', ' ');
      std::replace(line.begin(), line.end(), 'B', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal") {
          std::stringstream strstream(value);
          strstream >> memTotal;
        } else if(key == "MemFree") {
           std::stringstream strstream(value);
           strstream >> memFree;
        }
      }
    }
    utilization = memFree/ memTotal;
  }
  return utilization;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  string line, value;
  std::ifstream stream{kProcDirectory + kUptimeFilename};
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;
  }
  return std::stol(value);
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
	return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();
}

// TODO: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) { 
  string line;
  string  s_utime, ss_time, scu_time, scs_time;
  long jiffies;
  std::ifstream stream{kProcDirectory + to_string(pid) + kStatFilename};
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream{line};
    
    // Not get value from 0 to 13, at position 14 is utime
    for (int i = 0; i < 14; i++) linestream >> s_utime;

    linestream >> ss_time >> scu_time >> scs_time;

    jiffies = std::stol(s_utime) + std::stol(ss_time) + std::stol(scu_time) +
              std::stol(scs_time);
  }
  return jiffies / sysconf(_SC_CLK_TCK);
 }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
	string line, key;
  	long active;
  	long user, nice, system, idle, iowait, irq, softirq, steal;
  	std::ifstream myStream(kProcDirectory + kStatFilename);
  if (myStream.is_open()) {
    while (std::getline(myStream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> user >> nice >> system >> idle >> iowait >>
             irq >> softirq >> steal) {
        if (key == "cpu") {
          active = user + nice + system + irq + softirq + steal;
        }
      }
    }
  }
  return active / sysconf(_SC_CLK_TCK);
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  string line, key;
  long idleTime;
  long user, nice, system, idle, iowait, irq, softirq, steal;
  std::ifstream myStream(kProcDirectory + kStatFilename);
  if (myStream.is_open()) {
    while (std::getline(myStream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> user >> nice >> system >> idle >> iowait >>
             irq >> softirq >> steal) {
        if (key == "cpu") {
          idleTime = idle + iowait;
        }
      }
    }
  }
  return idleTime / sysconf(_SC_CLK_TCK);
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { return {}; }

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
	string line, key;
  	int num_proc;
    std::ifstream myStream(kProcDirectory + kStatFilename);
    if (myStream.is_open()) {
      while (std::getline(myStream, line)) {
        std::istringstream linestream(line);
        while (linestream >> key >> num_proc) {
          if (key == "processes") {
            return num_proc;
          }
        }
      }
    }
    return num_proc;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
	string line, key; 
  	int running;
  	std::ifstream myStream(kProcDirectory + kStatFilename);
    if (myStream.is_open()) {
      while (std::getline(myStream, line)) {
        std::istringstream linestream(line);
        while (linestream >> key >> running) {
          if (key == "procs_running") {
            return running;
          }
        }
      }
    }
    return running;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
	string line;
    std::ifstream myStream(kProcDirectory + to_string(pid) + kCmdlineFilename);
    if (myStream.is_open()) {
      std::getline(myStream, line);
    }
    return line;  
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
// Keyword: VmSize:
string LinuxParser::Ram(int pid) { 
	string line, key;
    float value;
    int ramTotal;
    std::ifstream myStream(kProcDirectory + to_string(pid) + kStatusFilename);
    if (myStream.is_open()) {
      while (std::getline(myStream, line)) {
        std::istringstream linestream(line);
        while (linestream >> key >> value) {
          if (key == "VmSize:") {
            ramTotal = value * 0.001;
          }
        }
      }
    }
    return to_string(ramTotal);
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  	string line,  key,  uid;
    std::ifstream myStream(kProcDirectory + to_string(pid) + kStatusFilename);
    if (myStream.is_open()) {
      while (std::getline(myStream, line)) {
        std::istringstream linestream(line);
        while (linestream >> key >> uid) {
          if (key == "Uid:") {
            return uid;
          }
        }
      }
    }
    return uid;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
	string line, user_name, pwd, uid;
    std::ifstream myStream(kPasswordPath);
    if (myStream.is_open()) {
      while (std::getline(myStream, line)) {
        std::replace(line.begin(), line.end(), ':', ' ');
        std::istringstream linestream(line);
        while (linestream >> user_name >> pwd >> uid) {
          if (uid == Uid(pid)) {
            return user_name;
          }
        }
      }
    }
    return user_name;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
	string line;
    string value;
    long uptime;
    std::ifstream stream{kProcDirectory + to_string(pid) + kStatFilename};
    if (stream.is_open()) {
      std::getline(stream, line);
      std::istringstream linestream{line};
      for (int i = 0; i < 22; i++) linestream >> value;
    }
    uptime = std::stol(value);
    uptime /= sysconf(_SC_CLK_TCK);
    return uptime;
}
