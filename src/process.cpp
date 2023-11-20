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

// Return this process's ID
int Process::Pid() { return m_pid; }

// Return this process's CPU utilization
float Process::CpuUtilization() const {
    /****************************** Calculation ******************************/
    // from /proc/uptime: uptime

    // from /proc/[PID]/stat:
    // #14 utime - CPU time spent in user code, measured in clock ticks
    // #15 stime - CPU time spent in kernel code, measured in clock ticks
    // #16 cutime - Waited-for children's CPU time spent in user code (in clock ticks)
    // #17 cstime - Waited-for children's CPU time spent in kernel code (in clock ticks)
    // #22 starttime - Time when the process started, measured in clock ticks

    // First we determine the total time spent for the process:
    // total_time = utime + stime
    // We also have to decide whether we want to include the time from children processes. If we do, then we add those values to total_time:
    // total_time = total_time + cutime + cstime
    // Next we get the total elapsed time in seconds since the process started:
    // seconds = uptime - (starttime / Hertz)
    // Finally we calculate the CPU usage percentage:
    // cpu_usage = 100 * ((total_time / Hertz) / seconds)
    /**************************************************************************/

    auto vec = LinuxParser::ProcessCpuUtilization(m_pid);

    auto totalTime = std::stol(vec[ProcessCPUTypes::UTIME]) + std::stol(vec[ProcessCPUTypes::STIME]);
    totalTime += std::stol(vec[ProcessCPUTypes::CUTIME]) + std::stol(vec[ProcessCPUTypes::CSTIME]);
    auto seconds = this->UpTime();

    auto cpuUsage = (totalTime / sysconf(_SC_CLK_TCK)) / (float)seconds;

    return cpuUsage;
}

// Return the command that generated this process
string Process::Command() {
    auto command = LinuxParser::Command(m_pid);
    const auto maxSize = 40;

    // Only print full commands with max 40 characters
    if (command.size() <= maxSize)
        return command;

    // Print only name of command if possible
    auto last = command.find_last_of('/');
    if (last != std::string::npos)
    {
        auto commandName = command.substr(last + 1);
        if (commandName.size() <= maxSize)
            return commandName;
        else
            return commandName.substr(0, maxSize - 3) + "...";
    }

    // If command to long, print only beginning
    return command.substr(0, maxSize - 3) + "...";
}

// Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(m_pid); }

// Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(m_pid); }

// Return the age of this process (in seconds)
long int Process::UpTime() const {
    auto processTicks = LinuxParser::UpTime(m_pid);
    auto processSeconds = processTicks / sysconf(_SC_CLK_TCK);
    auto uptime = LinuxParser::UpTime();

    if (uptime > processSeconds)
        return uptime - processSeconds;
    
    return uptime;
}

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
    return CpuUtilization() < a.CpuUtilization();
}