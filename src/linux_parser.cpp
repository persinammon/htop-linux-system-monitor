#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

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
      // note - why do they replace spaces with underscores?
      // answer to note: apparently newlines and " " are both
      // classifed as white space characters. according to the spec
      // in a whitespace delimited input the whitespaces will
      // become individual tokens. If a value has spaces in it,
      // the white space would break up the value into two lines
      // on the other hand, replace = and " with spaces will lead to 
      // only one value per line or only one value stripped from
      // useless double quotes per line, as the file is separated into KEY=VALUE or KEY="VALUE"
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

// This file has all information stored on one line, conveniently.
// Most of the files look more like the kOSPath above
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
// this gets every directory name that is an integer, currently using <dirent.h> lib
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



// Another design idea would be to read a file once, extract all relevant information,
// save it in some kind of variable (this isn't an instantiable class, so a private variable doesn't work).
// Then calls to metrics would either hit the cache and return pre-extracted information
// or call the general function that extracts all information in one go


// TODO: Read and return the system memory utilization

// I followed the original htop way to calculate memory
// utilization - MemTotal - MemFree
float LinuxParser::MemoryUtilization() { 
  string line;
  string key;
  float value;
  float memtotal;
  float memfree;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal") {
          memtotal = value;
        }
        if (key == "MemFree") {
          memfree = value;
        }
      }
    }
  }

  //learned about r-values vs l-values, guess this would be
  // an r-value?
  return memtotal - memfree;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  string line;
  long uptime;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return uptime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  long jiffies;
  vector<string> cpu_utilization{LinuxParser::CpuUtilization()};

  for (string s : cpu_utilization) {
    jiffies += std::stol(s);
  }

  return jiffies;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  long process_jiffies;
  string line;
  string c;
  string utime, stime, cutime, cstime, starttime;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);

    if (filestream.is_open()) {
      while (std::getline(filestream, line)) {
        std::istringstream linestream(line);
        for (int i = 0; i < 13; i++) {
          linestream >> c;
        }
        linestream >> utime >> stime >> cutime >> cstime >> starttime;
      }
  }
  process_jiffies = std::stol(utime) + std::stol(stime) + std::stol(cutime) + std::stol(cstime) + std::stol(starttime);

  return process_jiffies;

}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  long active_jiffies{0};

  vector<string> cpu_utilization{LinuxParser::CpuUtilization()};

  active_jiffies += std::stol(cpu_utilization[0]);
  active_jiffies += std::stol(cpu_utilization[1]);
  active_jiffies += std::stol(cpu_utilization[2]);
  active_jiffies += std::stol(cpu_utilization[5]);
  active_jiffies += std::stol(cpu_utilization[6]);
  active_jiffies += std::stol(cpu_utilization[7]);
  active_jiffies += std::stol(cpu_utilization[8]);
  active_jiffies += std::stol(cpu_utilization[9]);

  return active_jiffies;

}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  long idle_jiffies{0};
  vector<string> cpu_utilization{LinuxParser::CpuUtilization()};

  idle_jiffies += std::stol(cpu_utilization[3]);
  idle_jiffies += std::stol(cpu_utilization[4]);

  return idle_jiffies;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  vector<string> cpu_utilization;
    string line;
    string c, user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;

    std::ifstream filestream(kProcDirectory + kStatFilename);

    if (filestream.is_open()) {
      std::getline(filestream, line);
      std::istringstream linestream(line);
      linestream >> c >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice; 
      cpu_utilization.push_back(user);
      cpu_utilization.push_back(nice);
      cpu_utilization.push_back(system);
      cpu_utilization.push_back(idle);
      cpu_utilization.push_back(iowait);
      cpu_utilization.push_back(irq);
      cpu_utilization.push_back(softirq);
      cpu_utilization.push_back(steal);
      cpu_utilization.push_back(guest);
      cpu_utilization.push_back(guest_nice);
  }

  return cpu_utilization;

}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 

  string line;
  string key;
  int processes_num;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key == "processes") {
          linestream >> processes_num;
        }
      }
    }
  }

  return processes_num;
 
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 

  string line;
  string key;
  int running_processes_num;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key == "procs_running") {
          linestream >> running_processes_num;
        }
      }
    }
  }

  return running_processes_num;


}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  string command;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, command);
  }
  return " " + command;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  string line;
  string first;
  string value;
  string ram;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> first >> value) {
        if (first == "VmSize") {
          ram = value;
        }
      }
    }
  }

  return ram; 
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  string line;
  string first;
  string value;
  string uid;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> first >> value) {
        if (first == "Uid:") {
          uid = value;
        }
      }
    }
  }

  return uid; 
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  string uid = LinuxParser::Uid(pid);
  string tempName;
  string name = "User not found";
  string value;
  string line;

  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> tempName;
      while (linestream >> value) {
        if (value == uid) {
          name = tempName;
        }
      }
    }
  }
  return name; 
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
    long raw_uptime;
    string line;
    string c;

    std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);

    if (filestream.is_open()) {
      while (std::getline(filestream, line)) {
        std::istringstream linestream(line);
        for (int i = 0; i < 21; i++) {
          linestream >> c;
        }
        linestream >> raw_uptime;
      }
  }

  return raw_uptime / sysconf(_SC_CLK_TCK);
}
