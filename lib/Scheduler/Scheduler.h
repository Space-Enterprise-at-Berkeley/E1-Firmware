#pragma once

#include <Arduino.h>

#include <functional>
#include <queue>
#include <vector>

static const uint32_t HALF_HOUR = 34359738368;

using namespace std;

class Task {
public:
  virtual void run(uint32_t exec_time) = 0;
};

namespace Scheduler {
  struct Event {
    uint32_t when;
    uint32_t period; // microseconds between executions
    Task *task;
    void (*run)(uint32_t exec_time);

    bool operator >(const Event& a) const {
      if(when - a.when > HALF_HOUR) {
        return true;
      }
      return when > a.when;
    }
  };

  extern priority_queue<Event, vector<Event>, greater<Event>> eventq;

  void scheduleTask(Task *task, uint32_t when);
  void repeatTask(Task *task, uint32_t period);
  void scheduleFunc(void (*run)(uint32_t exec_time), uint32_t when);
  void repeatFunc(void (*run)(uint32_t exec_time), uint32_t period);
  void loop();
};
