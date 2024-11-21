#pragma once

#include <mysql.h>
#include <string.h>

#include <condition_variable>
#include <future>
#include <iostream>
#include <map>
#include <mutex>
#include <thread>
#include <vector>

#include "CircularBuffer/CircularBuffer.h"
#include "common/db_handler.h"

class MySQLArgs {
 public:
  MySQLArgs(std::string hostname, std::string user, std::string password, std::string db, int port)
      : hostname_(hostname), user_(user), password_(password), db_(db), port_(port) {}
  std::string hostname_;
  std::string user_;
  std::string password_;
  std::string db_;
  int port_;
  char* socket_name_ = nullptr;
};

class MysqlHandler {
 public:
  MysqlHandler(MySQLArgs args) : args_(args) {}
  ~MysqlHandler();
  bool Init();
  bool InitSchema(const std::map<std::string, std::string>& table, const std::map<std::string, std::string>& sp);
  bool Query(const std::string& sql);
  void Ping() { mysql_ping(conn_); }
  bool QueryAsync(const std::string& sql);
  static bool Query(MYSQL* conn, const std::string& sql);

 private:
  bool Query(const std::string& sql, uint32_t& ret);

 private:
  MYSQL* conn_ = nullptr;
  MySQLArgs args_;
};

using MySQLHandler = DBHandler<MysqlHandler, MySQLArgs>;