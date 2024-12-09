#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include "linux_parser.h"
#include "process.h"
#include "processor.h"
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// Constructor
System::System() { cpu_ = Processor(); }

//Done
// TODO: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// Done
// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() { 
  processes_.clear();
  auto pids = LinuxParser::Pids();
  for (int pid: pids ) {
    processes_.emplace_back(Process(pid));
  }
  for (Process &process : processes_){
      process.CpuInitialization();
  }
  std::sort(processes_.begin(), processes_.end());
  return processes_; 
}

// Done
// TODO: Return the system's kernel identifier (string)
std::string System::Kernel() { return LinuxParser::Kernel(); }

// Done
// TODO: Return the system's memory utilization
float System::MemoryUtilization() { return  LinuxParser::MemoryUtilization();}

// TODO: Return the operating system name
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// TODO: Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// TODO: Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// TODO: Return the number of seconds since the system started running
long System::UpTime() { return  LinuxParser::UpTime(); }