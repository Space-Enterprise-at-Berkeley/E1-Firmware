#include <Scheduler.h>

namespace Scheduler
{
  priority_queue<Event, vector<Event>, greater<Event>> eventq;

  void scheduleTask(Task *task, uint32_t when)
  {
    eventq.push((Event){.when = when, .task = task});
  }

  void repeatTask(Task *task, uint32_t period)
  {
    eventq.push((Event){.when = micros(), .period = period, .task = task});
  }

  void scheduleFunc(void (*run)(), uint32_t when)
  {
    eventq.push((Event){.when = when, .run = run});
  }

  void repeatFunc(void (*run)(), uint32_t period)
  {
    eventq.push((Event){.when = micros(), .period = period, .run = run});
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
