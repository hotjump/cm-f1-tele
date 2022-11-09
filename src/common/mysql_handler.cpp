#include "mysql_handler.h"
#include "loguru/loguru.hpp"

MysqlHandler::~MysqlHandler() {
  if (conn_ && connected_) {
    mysql_close(conn_);
    LOG_F(INFO, "close connection to database: %s:%d", host_.c_str(), port_);
  }
}

bool MysqlHandler::Init() {
  if ((conn_ = mysql_init(NULL)) == NULL) {
    LOG_F(ERROR, "init mysql client failed");
    return false;
  }

  bool reconnect = true;
  mysql_options(conn_, MYSQL_OPT_RECONNECT, &reconnect);

  if (mysql_real_connect(conn_, host_.c_str(), user_.c_str(), passwd_.c_str(), db_.c_str(), port_, socket_name_,
                         CLIENT_MULTI_STATEMENTS) == NULL) {
    mysql_close(conn_);
    LOG_F(ERROR, "mysql_real_connect failed: %s", mysql_error(conn_));
    return false;
  }

  connected_ = true;

  if (mysql_set_character_set(conn_, "utf8")) {
    mysql_close(conn_);
    LOG_F(ERROR, "mysql_set_character_set failed");
    return false;
  }

  if (mysql_autocommit(conn_, 0) != 0) {
    mysql_close(conn_);
    LOG_F(ERROR, "mysql_autocommit(conn_, 0) failed");
    return false;
  }

  Query("SET FOREIGN_KEY_CHECKS = 0;");
  LOG_F(INFO, "connection to database: %s:%d", host_.c_str(), port_);

  return true;
}

#define DBMRFREE(x)                                                         \
  {                                                                         \
    while (1) {                                                             \
      MYSQL_RES* r = mysql_store_result(x);                                 \
      if (r != NULL) mysql_free_result(r);                                  \
      int msg = mysql_next_result(x);                                       \
      if (msg > 0) {                                                        \
        LOG_F(ERROR, "mysql_next_result() failed with %s", mysql_error(x)); \
        return false;                                                       \
      } else if (msg < 0)                                                   \
        break;                                                              \
    }                                                                       \
  }

bool MysqlHandler::Query(const std::string& sql) {
  if (mysql_query(conn_, sql.c_str()) != 0) {
    LOG_F(ERROR, "Query failed with %s", mysql_error(conn_));
    return false;
  }

  DBMRFREE(conn_);

  if (mysql_commit(conn_) != 0) {
    LOG_F(ERROR, "Commit failed with %s", mysql_error(conn_));
    return false;
  }

  return true;
}

bool MysqlHandler::Query(const std::string& sql, uint32_t& ret) {
  DBMRFREE(conn_);

  if (mysql_query(conn_, sql.c_str()) != 0) {
    LOG_F(ERROR, "Commit failed with %s", mysql_error(conn_));
    return false;
  }

  MYSQL_RES* res = mysql_use_result(conn_);

  if (!res) {
    LOG_F(ERROR, "mysql_use_result failed");
    return false;
  }

  MYSQL_ROW row = mysql_fetch_row(res);
  ret = atoi(row[0]);

  return true;
}