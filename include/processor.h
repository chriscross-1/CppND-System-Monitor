#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <string>
#include <vector>

class Processor {
 public:
  float Utilization();

 private:
    std::vector<long> ConvertStringVecToLongVec(const std::vector<std::string>& vec) const;
    void SaveCurrentCPUStatistics(std::vector<long>& cpuStats);

    long m_prevIdle {0};
    long m_prevIoWait {0};
    long m_prevUser {0};
    long m_prevNice {0};
    long m_prevSystem {0};
    long m_prevIrq {0};
    long m_prevSoftIrq {0};
    long m_prevSteal {0};
};

#endif