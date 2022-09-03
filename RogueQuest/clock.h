#ifndef __CLOCK_H__
#define __CLOCK_H__

#include <chrono>

typedef std::chrono::time_point<std::chrono::system_clock> timepoint;

class Clock {
    timepoint start;
public:
    Clock();
    virtual ~Clock() = default;
    double getSeconds() const;
    double getMilliseconds() const;
    inline void restart() { start = std::chrono::system_clock::now(); }
    void setSeconds(double time);
    void setMilliseconds(double time);
};

#endif // __CLOCK_H__