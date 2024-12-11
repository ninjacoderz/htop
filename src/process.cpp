#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid): pid_(pid) {}

// TODO: Return this process's ID
int Process::Pid() const { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() const { 
  	long totalTime = LinuxParser::ActiveJiffies(this->Pid());
    long seconds = LinuxParser::UpTime() - Process::UpTime();
   	return (float)totalTime / seconds;
}
// TODO: Return the command that generated this process
string Process::Command() { 
	return LinuxParser::Command(Pid()); 
}

// TODO: Return this process's memory utilization
string Process::Ram() { 
	return LinuxParser::Ram(Pid());
}

// TODO: Return the user (name) that generated this process
string Process::User() { 
	return LinuxParser::User(Pid()); 
}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() const { return LinuxParser::UpTime(Pid()); }

// TODO: Overload the "less than" comparison operator for Process objects
// Use for sorting objects
bool Process::operator<(Process const& a) const {
	 return this->CpuUtilization() > a.CpuUtilization();
}