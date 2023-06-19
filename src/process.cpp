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

// TODO: Return this process's ID
int Process::Pid() { 
    return pid_;
}

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { 
    return ((float) LinuxParser::ActiveJiffies(pid_) / (float) LinuxParser::Jiffies()) * 100; 
}

// TODO: Return the command that generated this process
string Process::Command() { 
    if (command_ == "") {
        command_ = LinuxParser::Command(pid_);
        if (command_.size() > 50) command_ = command_.substr(0,49) + "...";
    }
    return command_; 
}

// TODO: Return this process's memory utilization
string Process::Ram() { 
    int ram = std::stoi(LinuxParser::Ram(pid_)) / 1024; // convert to MB
    return to_string(ram);
}

// TODO: Return the user (name) that generated this process
string Process::User() { 
    if (user_ == "") {
        user_ = LinuxParser::User(pid_);
    }
    return user_;
}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime() - LinuxParser::UpTime(pid_); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { 
    Process b {*this};
    return (b.CpuUtilization() < ((Process) a).CpuUtilization()); 
}