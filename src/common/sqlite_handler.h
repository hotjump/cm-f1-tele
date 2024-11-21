#pragma once

#include <sqlite3.h>

#include <map>
#include <string>

#include "common/db_handler.h"

class SqliteHandler {
 public:
  SqliteHandler(std::string db) : db_(db), db_path_("./db") {}
  ~SqliteHandler();
  bool Init();
  bool InitSchema(const std::map<std::string, std::string>& table, const std::map<std::string, std::string>& sp);
  bool Query(std::string sql);
  void Ping() {}

 private:
  sqlite3* conn_ = nullptr;
  const std::string db_;
  const std::string db_path_;
};

using SqliteDBHandler = DBHandler<SqliteHandler, std::string>;