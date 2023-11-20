#include "processor.h"
#include "linux_parser.h"

// Return the aggregate CPU utilization
float Processor::Utilization() {
    auto vec = LinuxParser::CpuUtilization();

    auto cpuStats = ConvertStringVecToLongVec(vec);

    /******************  CPU calculation  *******************/
    // PrevIdle = previdle + previowait
    // Idle = idle + iowait
    // PrevNonIdle = prevuser + prevnice + prevsystem +
    //               previrq + prevsoftirq + prevsteal
    // NonIdle = user + nice + system + irq + softirq + steal
    // PrevTotal = PrevIdle + PrevNonIdle
    // Total = Idle + NonIdle
    // # differentiate: actual value minus the previous one
    // totald = Total - PrevTotal
    // idled = Idle - PrevIdle
    // CPU_Percentage = (totald - idled)/totald
    /********************************************************/

    m_prevIdle = m_prevIdle + m_prevIoWait;
    auto idle = cpuStats[CPUType::IDLE] + cpuStats[CPUType::IOWAIT];

    auto prevNonIdle = m_prevUser + m_prevNice + m_prevSystem + m_prevIrq + m_prevSoftIrq + m_prevSteal;
    auto nonIdle = cpuStats[CPUType::USER] + cpuStats[CPUType::NICE] + cpuStats[CPUType::SYSTEM] +
                   cpuStats[CPUType::IRQ] + cpuStats[CPUType::SOFTIRQ] + cpuStats[CPUType::STEAL];

    auto prevTotal = m_prevIdle + prevNonIdle;
    auto total = idle + nonIdle;

    auto totald = total - prevTotal;
    auto idled = idle - m_prevIdle;

    auto cpuPercentage = (totald - idled) / (float)totald;

    SaveCurrentCPUStatistics(cpuStats);

    return cpuPercentage;
}

std::vector<long> Processor::ConvertStringVecToLongVec(const std::vector<std::string>& vec) const
{
    auto size = vec.size();
    std::vector<long> returnVec;
    returnVec.reserve(size);

    for (const auto& elem : vec)
    {
        returnVec.emplace_back(std::stol(elem));
    }

    return returnVec;
}

void Processor::SaveCurrentCPUStatistics(std::vector<long>& cpuStats)
{
    m_prevIdle = cpuStats[CPUType::IDLE];
    m_prevIoWait = cpuStats[CPUType::IOWAIT];
    m_prevUser = cpuStats[CPUType::USER];
    m_prevNice = cpuStats[CPUType::NICE];
    m_prevSystem = cpuStats[CPUType::SYSTEM];
    m_prevIrq = cpuStats[CPUType::IRQ];
    m_prevSoftIrq = cpuStats[CPUType::SOFTIRQ];
    m_prevSteal = cpuStats[CPUType::STEAL];
}