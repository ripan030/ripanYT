#ifndef EVENT_QUEUE
#define EVENT_QUEUE

#include <queue>
#include <mutex>
#include <condition_variable>

#include "Event.hpp"

class EventQueue {
  std::queue<Event> eventQueue_;
  std::mutex mutex_;
  std::condition_variable cv_;

public:
  // invoked from the main thread where event loop is running
  Event GetNextEvent() {
    std::unique_lock<std::mutex> lock{mutex_};

    // wait for the event
    cv_.wait(lock, [this](){return !eventQueue_.empty();});

    Event e = eventQueue_.front();

    eventQueue_.pop();

    return e;
  }

  // invoked from a different thread
  void PostEvent(const Event& e) {
    std::unique_lock<std::mutex> lock{mutex_};

    eventQueue_.push(e);

    cv_.notify_one();
  }
};

#endif /* EVENT_QUEUE */
