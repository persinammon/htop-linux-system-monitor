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

/*

These are generic utility functions that can
be called upon to parse files, depending on if 
it is just a value on a line or a key=value/
key then the following is value format.
This saves a lot of repetitive code per function,
leading to a decrease of potential errors.

*/

template <typename T> 
T findValueByKey(string const &keyFilter, string const &filename) {
  string line, key;
  T value;

  std::ifstream stream(filename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == keyFilter) {
          return value;
        }
      }
    }
  }
  stream.close();
  return value;
}

template <typename T>
T getValueOfFile(string const &filename) {
  string line;
  T value;

  std::ifstream stream(filename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;
  }
  stream.close();
  return value;
}

// Another design idea would be to read a file once, extract all relevant information,
// save it in some kind of variable (this isn't an instantiable class, so a private variable doesn't work).
// Then calls to metrics would either hit the cache and return pre-extracted information
// or call the general function that extracts all information in one go


// TODO: Read and return the system memory utilization

// I followed the original htop way to calculate memory
// utilization - MemTotal - MemFree
float LinuxParser::MemoryUtilization() { 
  float memTotal = findValueByKey<float>(filterMemTotalString, kProcDirectory + kMeminfoFilename);
  float memFree = findValueByKey<float>(filterMemFreeString, kProcDirectory + kMeminfoFilename);
  
  //learned about r-values vs l-values, guess this would be
  // an r-value?
  return (memTotal - memFree)/memTotal;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 

  long uptime = getValueOfFile<long>(kProcDirectory + kUptimeFilename);
  return uptime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  long jiffies{0L};
  vector<string> cpuUtilization = LinuxParser::CpuUtilization();

  for (string s : cpuUtilization) {
    jiffies += std::stol(s);
  }

  return jiffies;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  long processJiffies{0L};
  string line;
  string c;
  string utime, stime, cutime, cstime, starttime;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);

  if (filestream.is_open()) {
      std::getline(filestream, line);
      std::istringstream linestream(line);
      for (int i = 0; i < 12; i++) {
        linestream >> c;
      }
      linestream >> utime >> stime >> cutime >> cstime >> c >> c >> c >> c >> starttime;
  }
  filestream.close();
  processJiffies = std::stol(utime) + std::stol(stime) + std::stol(cutime) + std::stol(cstime) + std::stol(starttime);

  return processJiffies;

}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  long activeJiffies{0L};

  vector<string> cpuUtilization = LinuxParser::CpuUtilization();

  activeJiffies += std::stol(cpuUtilization[0]);
  activeJiffies += std::stol(cpuUtilization[1]);
  activeJiffies += std::stol(cpuUtilization[2]);
  activeJiffies += std::stol(cpuUtilization[5]);
  activeJiffies += std::stol(cpuUtilization[6]);
  activeJiffies += std::stol(cpuUtilization[7]);
  activeJiffies += std::stol(cpuUtilization[8]);
  activeJiffies += std::stol(cpuUtilization[9]);

  return activeJiffies;

}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  long idleJiffies{0L};
  vector<string> cpuUtilization = LinuxParser::CpuUtilization();

  idleJiffies += std::stol(cpuUtilization[3]);
  idleJiffies += std::stol(cpuUtilization[4]);

  return idleJiffies;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  vector<string> cpuUtilization;
  string line;
  string c, user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;

  std::ifstream filestream(kProcDirectory + kStatFilename);

  if (filestream.is_open()) {
      std::getline(filestream, line);
      std::istringstream linestream(line);
      linestream >> c >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice; 
      cpuUtilization.push_back(user);
      cpuUtilization.push_back(nice);
      cpuUtilization.push_back(system);
      cpuUtilization.push_back(idle);
      cpuUtilization.push_back(iowait);
      cpuUtilization.push_back(irq);
      cpuUtilization.push_back(softirq);
      cpuUtilization.push_back(steal);
      cpuUtilization.push_back(guest);
      cpuUtilization.push_back(guest_nice);
  }
  filestream.close();

  return cpuUtilization;

}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 

  int processesTotal = findValueByKey<int>(filterProcesses, kProcDirectory + kStatFilename);
  return processesTotal;
 
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 

  int runningProcessesTotal = findValueByKey<int>(filterRunningProcesses, kProcDirectory + kStatFilename);
  return runningProcessesTotal;

}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 

  string command = getValueOfFile<string>(kProcDirectory + to_string(pid) + kCmdlineFilename);
  return " " + command;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 

  /*
    VMSize gives the sum of all virtual memory, while VMRss gives the size of the exact 
    physical memory used as part of physical RAM. VMSize could even be bigger than total memory.
    Hence, switched it out to VmRSS.

    Link to documentation: https://man7.org/linux/man-pages/man5/proc.5.html
  */

  string ram = findValueByKey<string>(filterProcMem, kProcDirectory + to_string(pid) + kStatusFilename);
  return ram;

}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  string uid = findValueByKey<string>(filterUID, kProcDirectory + to_string(pid) + kStatusFilename);

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
  filestream.close();
  return name; 
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {

  long rawUptime;
  string line;
  string c;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);

  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 21; i++) {
        linestream >> c;
    }
    linestream >> rawUptime;
  }
  filestream.close();
  return rawUptime / sysconf(_SC_CLK_TCK);
}

