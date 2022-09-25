
#include "CyclicExecution.h"

CyclicExecution::CyclicExecution() {
    
}

void CyclicExecution::loop() {

    if (time() > nextCycle && !hadSpareTime) {      // CYCLES MISSED
        int missed = 0;
        while (time() > nextCycle) {
            missedCycles++;
            missed++;
            nextCycle += config.baseTickInterval;
        }
        alertCyclesMissed(missed);
        return;
    }

    if (nextCycle - time() < alert_us && !hadSpareTime) {   // CPU almost overloaded
        alertOverloaded();
    }

    if (time() < nextCycle) {
        hadSpareTime = true;
        return;
    }

    baseTick();

    nextCycle += config.baseTickInterval;
    baseTickCounter++;
    alert_us = config.baseTickInterval * (1.f - config.alertThreshold);
    hadSpareTime = false;
}

void CyclicExecution::baseTick() {
    if (baseTickCounter % task.tickNumber == 0) {
        uint32_t start = time();
        task.callback(&taskInfo);       // Execute the task
        taskInfo.prevTaskDuration = time() - start;
        taskInfo.firstCall = false;
        taskInfo.count++;
    }
}

void CyclicExecution::alertCyclesMissed(int cycles) {
    if (!config.enableAlerts)
        return;

    config.alertSerial->print("[ALERT] ");
    config.alertSerial->print(cycles);
    config.alertSerial->println(" cycles missed");
}

void CyclicExecution::alertOverloaded() {
    if (!config.enableAlerts)
        return;

    config.alertSerial->print("[Warning] CPU usage is above ");
    config.alertSerial->print(config.alertThreshold * 100);
    config.alertSerial->println("%");
}

void CyclicExecution::configure(struct Config config, struct Task task) {
    this->config = config;
    this->task = task;
    taskInfo = TaskInfo();
    nextCycle = time() + config.baseTickInterval;
}
