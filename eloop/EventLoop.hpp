#ifndef EVENT_LOOP
#define EVENT_LOOP

#include <unordered_map>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <optional>

#include "Event.hpp"
#include "EventQueue.hpp"

using EventHandler = std::function<void(const Event&)>;

class EventLoop {
  bool running_;
  std::unordered_map<Event::EventType, EventHandler> eventHandlers_;
  std::mutex mutex_;
  std::condition_variable cv_;

  EventQueue eventQueue_;

  void HandleEvent(const Event& e) {
    auto it = eventHandlers_.find(e.type);

    if (it != eventHandlers_.end()) {
      it->second(e);
    }
  }
public:
  EventLoop() : running_{true} {
    auto stopHandler = [this](const Event& e) {
      running_ = false;
    };

    RegisterHanlder(Event::EventType::QUIT, stopHandler);
  }

  void RegisterHanlder(Event::EventType type, EventHandler handler) {
    eventHandlers_[type] = handler;
  }

  // invoked from the main thread where event loop is running
  std::optional<Event> GetNextEvent() {
    std::unique_lock<std::mutex> lock{mutex_};

    // wait for the event
    cv_.wait_for(lock, std::chrono::milliseconds(100), [this](){return !eventQueue_.empty();});

    if (!eventQueue_.empty()) {
      Event e = eventQueue_.front();
      eventQueue_.pop();
      return e;
    }

    return std::nullopt;
  }

  // invoked from a different thread
  void PostEvent(const Event& e) {
    std::unique_lock<std::mutex> lock{mutex_};

    eventQueue_.push(e);

    cv_.notify_one();
  }

  void Run() {
    while (running_) {
      auto e = GetNextEvent();
      if (e) {
        HandleEvent(*e);
      }
    }
  }

  void Stop() {
    PostEvent({Event::EventType::QUIT});
  }
};

#endif /* EVENT_LOOP */
