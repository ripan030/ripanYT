#include <iostream>
#include <thread>

#include "EventLoop.hpp"

int main() {
  EventLoop loop;

  auto handler = [](const Event& e) {
    std::cout << "[main] Received event: " << e.GetId() << std::endl;
  };

  // run a thread to post event
  auto t = std::thread([&loop]() {
      std::chrono::milliseconds intvl{2000}; // 2sec
      std::this_thread::sleep_for(intvl);
      std::cout << "[Thread] posting event 1\n";
      loop.PostEvent({Event::EventType::EVENT1});
      std::this_thread::sleep_for(intvl);
      std::cout << "[Thread] posting event 2\n";
      loop.PostEvent({Event::EventType::EVENT2});
      std::this_thread::sleep_for(intvl);
      std::cout << "[Thread] invoke stop\n";
      loop.Stop();
      });

  loop.RegisterHanlder(Event::EventType::EVENT1, handler);
  loop.RegisterHanlder(Event::EventType::EVENT2, handler);

  loop.Run();

  if (t.joinable()) {
    t.join();
  }

  return 0;
}
