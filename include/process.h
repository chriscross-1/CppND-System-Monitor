#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
    Process(int pid) : m_pid(pid){}
    int Pid();
    std::string User();
    std::string Command();  
    float CpuUtilization() const;
    std::string Ram();
    long int UpTime() const;
    bool operator<(Process const& a) const;

 private:
    int m_pid {0};
};

#endif