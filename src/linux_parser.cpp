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
  std::string key;
  int memoryTotal = 1;
  int memoryFree = 1;
  std::string line;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);

  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "MemTotal:") {
        linestream >> memoryTotal;
      } else if (key == "MemFree:") {
        linestream >> memoryFree;
        break;
      } 
    }
  }

  return (memoryTotal - memoryFree) / (float) memoryTotal;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  long uptime;
  std::string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);

  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }

  return uptime;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string key;
  string value;
  std::vector<string> cpuTypes;
  cpuTypes.reserve(10);
  std::string line;
  std::ifstream stream(kProcDirectory + kStatFilename);

  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "cpu") {
        while (linestream >> value) {
          cpuTypes.emplace_back(value);
        }
        return cpuTypes;
      } 
    }
  }

  return cpuTypes;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::ProcessCpuUtilization(int pid) {
  string key;
  string value;
  std::vector<string> cpuTypes;
  cpuTypes.reserve(17);
  std::string line;
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kStatFilename);

  if (stream.is_open()) {
    if (std::getline(stream, line)) {
      std::istringstream linestream(line);
      for (int i = 0; i < 17; i++) {
        linestream >> value;
        cpuTypes.emplace_back(value);
      }
    }
  }

  return cpuTypes;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  return LinuxParser::GetValueFromStatFile("processes");
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  return LinuxParser::GetValueFromStatFile("procs_running");
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  std::string command;
  std::string line;
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kCmdlineFilename);

  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> command;
  }

  return command;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  std::string key;
  std::string value;
  long ram;
  std::string line;
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kStatusFilename);

  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "VmSize:") {
        linestream >> value;
        ram = std::stol(value);
        // Convert from KB to MB
        ram /= 1000;
        return std::to_string(ram);
      }
    }
  }
  return std::to_string(ram);
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  std::string key;
  std::string value;
  std::string line;
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kStatusFilename);

  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "Uid:") {
        linestream >> value;
        return value;
      }
    }
  }
  return value;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  std::string searchedUid = Uid(pid);
  std::string userName;
  std::string password;
  std::string uid;
  std::string line;
  std::ifstream stream(kPasswordPath);

  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> userName >> password >> uid;
      if (uid == searchedUid) {
        return userName;
      }
    }
  }
  return "N/A";
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  long uptime;
  std::string value;
  std::string line;
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kStatFilename);

  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 22; i++)
    {
      linestream >> value;
    }
    uptime = std::stol(value);
  }

  return uptime;
}

// Helper function
int LinuxParser::GetValueFromStatFile(const std::string& key)
{  
  std::string currentKey;
  std::string line;
  int value;
  std::ifstream stream(kProcDirectory + kStatFilename);

  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> currentKey;
      if (currentKey == key) {
        linestream >> value;
        return value;
      }
    }
  }
  return value;
}
