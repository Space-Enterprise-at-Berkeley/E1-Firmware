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

  // // Remove this later
  // // specify task object, and task member function to run
  // void scheduleTask(Task *task, uint32_t when, taskfunc run)
  // {
  //   eventq.push((Event){.when = when, .period = 0, .task = task, .taskrun = run, .run =nullptr});
  // }

  // void repeatTask(Task *task, uint32_t period, taskfunc run)
  // {
  //   eventq.push((Event){.when = micros(), .period = period, .task = task, .taskrun = run, .run = nullptr});
  // }

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
    if(!eventq.empty() && eventq.top().when <= now) {
      Serial.println("popping valid event. total events:\t" + eventq.size());
      Event top = eventq.top();
      eventq.pop();
      if(top.period > 0) {
        top.when = now + top.period;
        eventq.push(top);
      }
      if(top.task != nullptr) {
        Serial.println("running task " + (unsigned long)(top.task));
        top.task->run(now);
      } else {
        top.run(now);
      }
    }
  }
};
