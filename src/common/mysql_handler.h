#pragma once

#include <mysql.h>
#include <string.h>
#include <condition_variable>
#include <future>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "CircularBuffer/CircularBuffer.h"

class MysqlHandler {
 public:
  MysqlHandler(const std::string& hostname, const std::string& user, const std::string& password, const std::string& db,
               int mysql_port, size_t thread_num)
      : host_(hostname), user_(user), passwd_(password), db_(db), port_(mysql_port), thread_num_(thread_num) {}
  ~MysqlHandler();
  bool Init();
  bool QueryAsync(const std::string& sql);
  bool Query(const std::string& sql);
  static bool InitMysqlConn(MYSQL*& conn, const std::string& host, const std::string& user, const std::string& passwd,
                            const std::string& db, const int& port, const char* socket_name);
  static bool Query(MYSQL* conn, const std::string& sql);

 private:
  void ConsumeQueryThread(int idx, std::promise<bool>& promise_obj);
  bool Query(const std::string& sql, uint32_t& ret);

 private:
  MYSQL* conn_ = nullptr;

  const std::string host_;    //服务器主机名称 默认为localhost
  const std::string user_;    //数据库用户名 默认为当前登录名
  const std::string passwd_;  //密码
  const std::string db_;
  const int port_;                     //端口
  const char* socket_name_ = nullptr;  // socket名

  CircularBuffer<std::string, 1024> sql_buffer_;
  const size_t thread_num_ = 2;
  std::mutex mutex_;
  std::condition_variable cv_;
  std::vector<std::thread> sql_query_threads_;
  bool running_ = true;
};