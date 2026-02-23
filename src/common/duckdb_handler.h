#pragma once

#include "duckdb.hpp"

#include <map>
#include <string>

#include "common/db_handler.h"

class DuckDBHandler {
 public:
  DuckDBHandler(std::string db) : db_(db), db_path_("./db") {}
  ~DuckDBHandler();
  bool Init();
  bool InitSchema(const std::map<std::string, std::string>& table, const std::map<std::string, std::string>& sp);
  bool Query(std::string sql);
  void Ping() {}

 private:
  std::unique_ptr<duckdb::DuckDB> db_instance_ = nullptr;
  std::unique_ptr<duckdb::Connection> conn_ = nullptr;
  const std::string db_;
  const std::string db_path_;
};

using DuckDBHandlerWrapper = DBHandler<DuckDBHandler, std::string>;