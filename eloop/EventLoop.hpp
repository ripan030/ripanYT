#ifndef EVENT_LOOP
#define EVENT_LOOP

#include <unordered_map>

#include "Event.hpp"
#include "EventQueue.hpp"

using EventHandler = std::function<void(const Event&)>;

class EventLoop {
  bool running_;
  std::unordered_map<Event::EventType, EventHandler> eventHandlers_;
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

  void PostEvent(const Event& e) {
    eventQueue_.PostEvent(e);
  }

  void RegisterHanlder(Event::EventType type, EventHandler handler) {
    eventHandlers_[type] = handler;
  }

  void Run() {
    while (running_) {
      auto e = eventQueue_.GetNextEvent();
      HandleEvent(e);
    }
  }

  void Stop() {
    PostEvent({Event::EventType::QUIT});
  }
};

#endif /* EVENT_LOOP */
