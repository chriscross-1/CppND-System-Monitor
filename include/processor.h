#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <string>
#include <vector>

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

 private:
    // TODO: Declare any necessary private members
    std::vector<int> ConvertStringVecToIntVec(const std::vector<std::string>& vec) const;
    void SaveCurrentCPUStatistics(std::vector<int>& cpuStats);

    int m_prevIdle {0};
    int m_prevIoWait {0};
    int m_prevUser {0};
    int m_prevNice {0};
    int m_prevSystem {0};
    int m_prevIrq {0};
    int m_prevSoftIrq {0};
    int m_prevSteal {0};
};

#endif