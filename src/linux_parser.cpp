#include <dirent.h>
#include <unistd.h>
#include <iostream>
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
  string os, version, kernel;
  string line;
  std::ifstream filestream(kProcDirectory + kVersionFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
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

float LinuxParser::MemoryUtilization() {
  float total_memory, free_memory;
  string key, value, unit, line;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value >> unit) {
        if (key == "MemTotal") {
          total_memory = stof(value);
        }
        if (key == "MemFree") {
          free_memory = stof(value);
        }
      }
    }
  }
  return (total_memory - free_memory) / total_memory;
}

long LinuxParser::UpTime() {
  long uptime_long;
  string uptime, idletime;
  string line;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime >> idletime;
    uptime_long = stol(uptime);
  }
  return uptime_long;
}

long LinuxParser::ActiveJiffies(int pid) {
  string value, line;
  long total_time;
  vector<string> stats;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      stats.push_back(value);
    }
  }
  long int utime = stol(stats[13]);
  long int stime = stol(stats[14]);
  long int cutime = stol(stats[15]);
  long int cstime = stol(stats[16]);
  total_time = (utime + stime + cutime + cstime);
  return total_time;
}

vector<string> LinuxParser::CpuUtilization() {
  string value, key;
  string line;
  vector<string> jiffies;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> key) {
      if (key != "cpu") {
        jiffies.push_back(key);
      }
    }
  }
  return jiffies;
}

int LinuxParser::TotalProcesses() {
  int num_total_processes;
  string key, value, line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          num_total_processes = stoi(value);
        }
      }
    }
  }
  return num_total_processes;
}

int LinuxParser::RunningProcesses() {
  int num_running_processes;
  string key, value, line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          num_running_processes = stoi(value);
        }
      }
    }
  }
  return num_running_processes;
}

string LinuxParser::Command(int pid) {
  string cmd, line;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> cmd;
  }
  return cmd;
}

string LinuxParser::Ram(int pid) {
  string key, value, unit, line;
  string ram;
  int vmsize;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value >> unit) {
        if (key == "VmSize:") {
          vmsize = stoi(value);
          vmsize = vmsize / 1024;
          ram = to_string(vmsize);
        }
      }
    }
  }
  return ram;
}

string LinuxParser::Uid(int pid) {
  string line;
  string key, value;
  string uid;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {
          uid = value;
        }
      }
    }
  }
  return uid;
}

string LinuxParser::User(int pid) {
  string user, current_user, password, current_uid, line;
  string uid = LinuxParser::Uid(pid);
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> current_user >> password >> current_uid) {
        if (uid == current_uid) {
          user = current_user;
        }
      }
    }
  }
  return user;
}

long LinuxParser::UpTime(int pid) {
  string value, line;
  vector<string> stats;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      stats.push_back(value);
    }
  }
  long int starttime = stol(stats[21]) / sysconf(_SC_CLK_TCK);
  long int uptime = LinuxParser::UpTime() - starttime;
  return uptime;
}

float LinuxParser::CpuUtilization(int pid) {
  long active_time = LinuxParser::ActiveJiffies(pid) / sysconf(_SC_CLK_TCK);
  long uptime = LinuxParser::UpTime(pid);
  return (float)active_time / uptime;
}