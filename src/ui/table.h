#pragma once

#include <ftxui/component/component.hpp>

#include "common/mysql_handler.h"
#include "ftxui/component/component_base.hpp"  // for ComponentBase

class TableGrid : public ftxui::ComponentBase {
 public:
  TableGrid(std::shared_ptr<MySQLHandler> my) : my_(my) {}
  virtual ~TableGrid() {}
  TableGrid(std::string sql) : sql_(sql) {}
  ftxui::Element Render() override;

 private:
  std::string sql_;
  std::shared_ptr<MySQLHandler> my_;
};