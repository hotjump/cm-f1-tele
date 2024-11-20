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

class MySQLArgs {
 public:
  std::string hostname;  // 服务器主机名称 默认为localhost
  std::string user;      // 数据库用户名 默认为当前登录名
  std::string password;  // 密码
  std::string db;
  int port;                     // 端口
  char* socket_name = nullptr;  // socket名
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