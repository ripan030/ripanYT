#ifndef EVENT_HPP
#define EVENT_HPP

#include <functional>

struct Event {
  enum class EventType {
    QUIT = 0,
    EVENT1,
    EVENT2,
    EVENT3,
  };

  EventType type;

  Event(EventType t) : type{t} {}

  int GetId() const {
    return static_cast<int>(type);
  }
};

#endif /* EVENT_HPP */
