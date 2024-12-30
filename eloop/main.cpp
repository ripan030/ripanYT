#include <iostream>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <condition_variable>

enum class EventType {
  EVENT1 = 1,
  EVENT2 = 2,
};

using EventHanlder = std::function<void(const EventType)>;

class EventLoop {
  bool running_;
  std::unordered_map<EventType, EventHanlder> eventHanlders_;


  bool eventReceived_;
  EventType incomingEvent_;
  std::mutex mutex_;
  std::condition_variable cv_;
public:
  EventLoop() : running_{true} {

  }

  void RegisterHanlder(EventType e, EventHanlder handler) {
    eventHanlders_[e] = handler;
  }

  // invoked from the main thread where event loop is running
  EventType GetNextEvent() {
    std::unique_lock<std::mutex> lock{mutex_};

    // wait for the event
    cv_.wait(lock, [this](){return this->eventReceived_ == true;});

    eventReceived_ = false;

    return incomingEvent_;
  }

  // invoked from a different thread
  void PostEvent(EventType e) {
    std::unique_lock<std::mutex> lock{mutex_};

    eventReceived_ = true;

    incomingEvent_ = e;

    cv_.notify_one();
  }

  void HandleEvent(EventType e) {
    auto it = eventHanlders_.find(e);

    if (it != eventHanlders_.end()) {
      it->second(e);
    }
  }

  void Run() {
    while (running_) {
      std::cout << "[main] waiting for the event..\n";
      EventType e = GetNextEvent();
      HandleEvent(e);
    }
  }
};

#include <thread>

int main() {
  EventLoop loop;

  auto handler = [](const EventType e) {
    std::cout << "[main] Received event: " << static_cast<int>(e) << std::endl;
  };

  // run a thread to post event
  auto thread_ = std::thread([&loop]() {
      std::chrono::milliseconds intvl{2000}; // 2sec
      std::this_thread::sleep_for(intvl);
      std::cout << "[Thread] posting event\n";
      loop.PostEvent(EventType::EVENT1);
      std::this_thread::sleep_for(intvl);
      loop.PostEvent(EventType::EVENT2);
      });

  loop.RegisterHanlder(EventType::EVENT1, handler);
  loop.RegisterHanlder(EventType::EVENT2, handler);

  loop.Run();

  return 0;
}
