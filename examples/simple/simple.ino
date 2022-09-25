
// #define MAX_TASK_NUMBER x

#include "CyclicExecution.h"

CyclicExecution executor;
uint32_t elapsed = 0;

void slowTask() {
    static bool state = false;
    digitalWrite(13, state);
    state = !state;

    Serial.print("Elapsed: ");
    Serial.println(elapsed);
}

void fastTask() {
    static bool state = false;
    digitalWrite(8, state);
    state = !state;

    static uint32_t old = 0;
    uint32_t now = micros();
    elapsed = now - old;
    old = now;
}

void task(TaskInfo* info) {
    fastTask();
    if (info->count % 1000 == 0) { 
        slowTask();
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(8, OUTPUT);
    pinMode(13, OUTPUT);

    struct Config config;
    config.baseTickInterval = 500;      // [us] Base tick interval
    config.alertThreshold = 0.8f;       // Warn if cpu usage is above 80%
    
    struct Task _task;
    _task.callback = task;
    _task.tickNumber = 1;

    executor.configure(config, _task);

}

void loop() { 
    executor.loop(); 
}