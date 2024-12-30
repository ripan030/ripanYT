#include <iostream>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <queue>

enum class EventType {
  EVENT1 = 1,
  EVENT2 = 2,
  EVENT3 = 3,
};

using EventHanlder = std::function<void(const EventType)>;

class EventLoop {
  bool running_;
  std::unordered_map<EventType, EventHanlder> eventHanlders_;


  std::queue<EventType> eventQueue;
  std::mutex mutex_;
  std::condition_variable cv_;
public:
  EventLoop() : running_{true} {
    RegisterHanlder(EventType::EVENT3, [this](const EventType e){this->running_ = false;});
  }

  void RegisterHanlder(EventType e, EventHanlder handler) {
    eventHanlders_[e] = handler;
  }

  // invoked from the main thread where event loop is running
  EventType GetNextEvent() {
    std::unique_lock<std::mutex> lock{mutex_};

    // wait for the event
    cv_.wait(lock, [this](){return !eventQueue.empty();});

    EventType e = eventQueue.front();

    eventQueue.pop();

    return e;
  }

  // invoked from a different thread
  void PostEvent(EventType e) {
    std::unique_lock<std::mutex> lock{mutex_};

    eventQueue.push(e);

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

  void Stop() {
    PostEvent(EventType::EVENT3);
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
      std::this_thread::sleep_for(intvl);
      std::cout << "[Thread] invoke stop\n";
      loop.Stop();
      });

  loop.RegisterHanlder(EventType::EVENT1, handler);
  loop.RegisterHanlder(EventType::EVENT2, handler);

  loop.Run();

  if (thread_.joinable()) {
    thread_.join();
  }

  return 0;
}
