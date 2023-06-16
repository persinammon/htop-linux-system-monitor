#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

/* Pass through functions for metrics that can change, cached values for member variables that don't. */

#include "linux_parser.h"

// TODO: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() { return processes_; }

// TODO: Return the system's kernel identifier (string)
std::string System::Kernel() { 
    if (kernel_ == "") {
        kernel_ = LinuxParser::Kernel();
    } 
    return kernel_;
}

// TODO: Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// TODO: Return the operating system name
std::string System::OperatingSystem() { 
    if (os_ == "") {
        os_ = LinuxParser::OperatingSystem();
    } 
    return os_;
}

// TODO: Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// TODO: Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses();  }

// TODO: Return the number of seconds since the system started running
// ncurses calles a formatting function when displaying the long as a string
long int System::UpTime() { return LinuxParser::UpTime(); }
