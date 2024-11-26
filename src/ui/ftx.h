#pragma once

#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/string.hpp>
#include <memory>

#include "common/mysql_handler.h"
#include "ftxui/component/captured_mouse.hpp"      // for ftxui
#include "ftxui/component/component.hpp"           // for Dropdown, Horizontal, Vertical
#include "ftxui/component/component.hpp"           // for Radiobox, Renderer, Tab, Toggle, Vertical
#include "ftxui/component/component_base.hpp"      // for ComponentBase
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/elements.hpp"                  // for Element, separator, operator|, vbox, border
#include "loguru/loguru.hpp"
#include "ui/table.h"

class FTXUI {
 public:
  FTXUI(std::shared_ptr<MySQLHandler> my) { table_ = ftxui::Make<TableGrid>(my); }
  ~FTXUI();
  void Run() { screen_.Loop(table_); }
  void Init();

 private:
  void TimerThread();
  bool running_ = true;
  std::thread timer_;
  ftxui::ScreenInteractive screen_ = ftxui::ScreenInteractive::Fullscreen();
  ftxui::Component table_ = nullptr;
};