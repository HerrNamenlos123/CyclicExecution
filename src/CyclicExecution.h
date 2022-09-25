
#ifndef _CYCLIC_EXECUTION_H
#define _CYCLIC_EXECUTION_H

#include "Arduino.h"

struct TaskInfo {
    bool firstCall = true;              // True for only the first cycle, can be used for initialization
    uint32_t actualInterval = 0;        // Time elapsed since the task was called the last time
    uint32_t prevTaskDuration = 0;      // How long the task took to execute the last time [us]
    uint32_t count = 0;                 // Counter how often the task was executed
};

struct Config {
    uint32_t baseTickInterval = 0;          // Base tick interval in us
    float alertThreshold = 0.8f;            // Percentage at which to set a warning when cpu overloaded
    bool enableAlerts = true;               // If timing alerts should be printed to the console
    Stream* alertSerial = &Serial;          // Serial stream for alert output
};

struct Task {
    void (*callback)(TaskInfo* info) = nullptr;   // Function pointer to your task to be executed 
    uint32_t tickNumber = 0;                      // Number of base ticks between task calls
};

class CyclicExecution {
public:
    CyclicExecution();

    void loop();

    void configure(struct Config config, struct Task task);

private:
    uint32_t time() { return micros(); }    // This function could be replaced here
    void baseTick();

    void alertCyclesMissed(int cycles);
    void alertOverloaded();

private:
    Config config;
    uint32_t baseTickCounter = 0;
    uint32_t nextCycle = 0;
    uint32_t lastCycle = 0;
    uint32_t missedCycles = 0;
    bool hadSpareTime = false;
    uint32_t alert_us = 0;

    Task task;
    TaskInfo taskInfo;
};

#endif // _CYCLIC_EXECUTION_H
