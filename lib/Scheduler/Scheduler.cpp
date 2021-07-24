#include <Scheduler.h>

namespace Scheduler
{
  priority_queue<Event, vector<Event>, greater<Event>> eventq;

  void scheduleTask(Task *task, uint32_t when)
  {
    Event e;
    e.when = when;
    e.period = 0;
    e.task = task;
    e.run = nullptr;
    eventq.push(e);
    // eventq.push((Event){.when = when, .period = 0, .task = task, .run = nullptr});
  }

  void repeatTask(Task *task, uint32_t period)
  {
    Event e;
    e.when = micros();
    e.period = period;
    e.task = task;
    e.run = nullptr;
    eventq.push(e);
  }

  void scheduleFunc(void (*run)(), uint32_t when)
  {
    Event e;
    e.when = when;
    e.period = 0;
    e.task = nullptr;
    e.run = run;
    eventq.push(e);
  }

  void repeatFunc(void (*run)(), uint32_t period)
  {
    Event e;
    e.when = micros();
    e.period = period;
    e.task = nullptr;
    e.run = run;
    eventq.push(e);
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
        top.task->run();
      } else {
        top.run();
      }
    }
  }
};
