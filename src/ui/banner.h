#pragma once

#include <ftxui/component/component.hpp>

#include "common/mysql_handler.h"
#include "ftxui/component/component_base.hpp"  // for ComponentBase

class Banner : public ftxui::ComponentBase {
 public:
  Banner() {}
  Banner(std::shared_ptr<MySQLHandler> my) : my_(my) {}

  ftxui::Element Render() override;

 private:
  /* data */
  std::shared_ptr<MySQLHandler> my_;
  int index_ = 0;
};
