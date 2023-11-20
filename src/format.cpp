#include <string>
#include <iomanip>
#include <iostream>

#include "format.h"
#include "linux_parser.h"

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    int secondsLeft = seconds % 60;

    std::ostringstream stream;
    stream << std::setfill('0') << std::setw(2) << hours << ":" 
           << std::setw(2) << minutes << ":"
           << std::setw(2) << secondsLeft;

    return stream.str();
}