#include <Scheduler.h>

namespace Scheduler
{
  priority_queue<Event, vector<Event>, greater<Event>> eventq;

  void scheduleTask(Task *task, uint32_t when)
  {
    eventq.push((Event){.when = when, .period = 0, .task = task, .run = nullptr});
  }

  void repeatTask(Task *task, uint32_t period)
  {
    eventq.push((Event){.when = micros(), .period = period, .task = task, .run = nullptr});
  }

  void scheduleFunc(void (*run)(uint32_t exec_time), uint32_t when)
  {
    eventq.push((Event){.when = when, .period = 0, .task = nullptr, .run = run});
  }

  void repeatFunc(void (*run)(uint32_t exec_time), uint32_t period)
  {
    eventq.push((Event){.when = micros(), .period = period, .task = nullptr, .run = run});
  }

  void loop()
  {
    uint32_t now = micros();
    if(eventq.top().when <= now) {
      Event top = eventq.top();
      eventq.pop();
      if(top.period > 0) {
        top.when = now + top.period;
        eventq.push(top);
      }
      if(top.task != nullptr) {
        top.task->run(now);
      } else {
        top.run(now);
      }
    }
  }
};
