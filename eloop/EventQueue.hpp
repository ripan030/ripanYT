#ifndef EVENT_QUEUE
#define EVENT_QUEUE

#include <queue>
#include <mutex>
#include <condition_variable>

#include "Event.hpp"

class EventQueue {
  std::queue<Event> eventQueue_;

public:
  void push(const Event& e) {
    eventQueue_.push(e);
  }

  void pop() {
    eventQueue_.pop();
  }

  Event front() {
    return eventQueue_.front();
  }

  bool empty() {
    return eventQueue_.empty();
  }
};

#endif /* EVENT_QUEUE */
