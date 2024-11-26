#include "ui/ftx.h"

#include "loguru/loguru.hpp"

FTXUI::~FTXUI() {
  running_ = false;
  timer_.join();
}

void FTXUI::Init() {
  auto timer = std::bind(&FTXUI::TimerThread, this);
  timer_ = std::thread(timer);
}

void FTXUI::TimerThread() {
  while (running_) {
    // Refresh the screen:
    screen_.PostEvent(ftxui::Event::Custom);
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(0.5s);
  }
}