#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();

 private:
  float total_jiffies{0.0};
  float total_jiffies_prev{0.0};
  float idle_jiffies{0.0};
  float util_jiffies{0.0};
  float util_jiffies_prev{0.0};
  float usage{0.0};
};

#endif