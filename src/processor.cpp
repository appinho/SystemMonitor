#include "processor.h"
#include "linux_parser.h"

float Processor::Utilization() {
  std::vector<std::string> jiffies = LinuxParser::CpuUtilization();
  for (int i = 0; i < int(jiffies.size()); i++) {
    if (i != LinuxParser::kGuest_ and i != LinuxParser::kGuestNice_) {
      total_jiffies += std::stof(jiffies[i]);
    }
    if (i == LinuxParser::kIdle_ or i == LinuxParser::kIOwait_) {
      idle_jiffies += std::stof(jiffies[i]);
    }
  }
  util_jiffies = total_jiffies - idle_jiffies;

  bool curr_util = true;
  if (curr_util) {
    usage = (util_jiffies - util_jiffies_prev) /
            (total_jiffies - total_jiffies_prev);
    total_jiffies_prev = total_jiffies;
    util_jiffies_prev = util_jiffies;
  } else {
    usage = util_jiffies / total_jiffies;
  }

  return usage;
}