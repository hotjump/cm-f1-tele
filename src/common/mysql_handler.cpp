#include "mysql_handler.h"

MysqlHandler::~MysqlHandler() {
  if (conn_ && connected_) {
    mysql_close(conn_);
    std::cout << "[success] close connection to database: " << host_ << ":" << port_ << std::endl;
  }
}

bool MysqlHandler::Init() {
  if ((conn_ = mysql_init(NULL)) == NULL) {
    std::cout << "init mysql client failed" << std::endl;
    return false;
  }

  bool reconnect = 0;
  mysql_options(conn_, MYSQL_OPT_RECONNECT, &reconnect);

  if (mysql_real_connect(conn_, host_.c_str(), user_.c_str(), passwd_.c_str(), db_.c_str(), port_, socket_name_,
                         CLIENT_MULTI_STATEMENTS) == NULL) {
    std::cout << "mysql_real_connect failed" << std::endl;
    mysql_close(conn_);
    return false;
  }

  connected_ = true;

  if (mysql_set_character_set(conn_, "utf8")) {
    std::cout << "mysql_set_character_set failed" << std::endl;
    mysql_close(conn_);
    return false;
  }

  if (mysql_autocommit(conn_, 0) != 0) {
    std::cout << "mysql_real_connect failed" << std::endl;
    mysql_close(conn_);
    return false;
  }

  std::cout << "[success] Connect database: " << host_ << ":" << port_ << std::endl;

  return true;
}

#define DBMRFREE(x)                                                                               \
  {                                                                                               \
    while (1) {                                                                                   \
      MYSQL_RES* r = mysql_store_result(x);                                                       \
      if (r != NULL) mysql_free_result(r);                                                        \
      int msg = mysql_next_result(x);                                                             \
      if (msg > 0) {                                                                              \
        std::cout << "[failed]: mysql_next_result() failed with " << mysql_error(x) << std::endl; \
        return false;                                                                             \
      } else if (msg < 0)                                                                         \
        break;                                                                                    \
    }                                                                                             \
  }

bool MysqlHandler::Query(const std::string& sql) {
  if (mysql_query(conn_, sql.c_str()) != 0) {
    std::cout << "[failed]: " << sql << std::endl;
    std::cout << "[failed]: " << mysql_error(conn_) << std::endl;
    return false;
  }

  DBMRFREE(conn_);

  if (mysql_commit(conn_) != 0) {
    std::cout << "[failed]: " << sql << std::endl;
    std::cout << "[failed]: " << mysql_error(conn_) << std::endl;
    return false;
  }

  return true;
}

bool MysqlHandler::Query(const std::string& sql, uint32_t& ret) {
  DBMRFREE(conn_);

  // mysql_ping(conn_);

  if (mysql_query(conn_, sql.c_str()) != 0) {
    std::cout << "[failed]: " << sql << std::endl;
    return false;
  }

  MYSQL_RES* res = mysql_use_result(conn_);

  if (!res) {
    std::cout << "[failed]: mysql_use_result failed" << std::endl;
    return false;
  }

  MYSQL_ROW row = mysql_fetch_row(res);
  ret = atoi(row[0]);

  return true;
}