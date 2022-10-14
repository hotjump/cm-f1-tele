#pragma once

#include <mysql.h>
#include <string.h>
#include <iostream>

class MysqlHandler {
 public:
  MysqlHandler(const std::string& hostname, const std::string& user, const std::string& password, const std::string& db,
               int mysql_port)
      : host_(hostname), user_(user), passwd_(password), db_(db), port_(mysql_port) {}
  ~MysqlHandler();
  bool Init();
  bool Query(const std::string& sql);
  bool Query(const std::string& sql, uint32_t& ret);

 private:
  MYSQL* conn_;

  bool connected_ = false;

  const std::string host_;    //服务器主机名称 默认为localhost
  const std::string user_;    //数据库用户名 默认为当前登录名
  const std::string passwd_;  //密码
  const std::string db_;
  const int port_;                  //端口
  const char* socket_name_ = NULL;  // socket名
};