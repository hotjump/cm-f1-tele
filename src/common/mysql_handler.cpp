#include "mysql_handler.h"

#include <functional>

#include "loguru/loguru.hpp"

#define DBMRFREE(x)                                                         \
  {                                                                         \
    while (1) {                                                             \
      MYSQL_RES* r = mysql_store_result(x);                                 \
      if (r != NULL) mysql_free_result(r);                                  \
      int msg = mysql_next_result(x);                                       \
      if (msg > 0) {                                                        \
        LOG_F(ERROR, "mysql_next_result() failed with %s", mysql_error(x)); \
        LOG_F(ERROR, "mysql_next_result() failed with %s", sql.c_str());    \
        return false;                                                       \
      } else if (msg < 0)                                                   \
        break;                                                              \
    }                                                                       \
  }

bool DBResFree(MYSQL* x) {
  while (1) {
    MYSQL_RES* r = mysql_store_result(x);
    if (r != NULL) mysql_free_result(r);
    int msg = mysql_next_result(x);
    if (msg > 0) {
      LOG_F(ERROR, "mysql_next_result() failed with %s", mysql_error(x));
      return false;
    } else if (msg < 0)
      break;
  }
  return true;
}

MysqlHandler::~MysqlHandler() {
  if (conn_) {
    mysql_close(conn_);
    LOG_F(INFO, "close connection to database: %s:%d", args_.hostname_.c_str(), args_.port_);
  }
}

bool MysqlHandler::Init() {
  if ((conn_ = mysql_init(NULL)) == NULL) {
    LOG_F(ERROR, "init mysql client failed");
    return false;
  }

  bool reconnect = true;
  mysql_options(conn_, MYSQL_OPT_RECONNECT, &reconnect);

  if (mysql_real_connect(conn_, args_.hostname_.c_str(), args_.user_.c_str(), args_.password_.c_str(), nullptr,
                         args_.port_, args_.socket_name_, CLIENT_MULTI_STATEMENTS) == NULL) {
    LOG_F(ERROR, "mysql_real_connect failed: %s", mysql_error(conn_));
    mysql_close(conn_);
    conn_ = nullptr;
    return false;
  }

  if (mysql_set_character_set(conn_, "utf8")) {
    LOG_F(ERROR, "mysql_set_character_set failed");
    mysql_close(conn_);
    conn_ = nullptr;
    return false;
  }

  // if (mysql_autocommit(conn_, 0) != 0) {
  //  LOG_F(ERROR, "mysql_autocommit(conn_, 0) failed");
  // mysql_close(conn_);
  // conn_ = nullptr;
  // return false;
  //}

  LOG_F(INFO, "connection to database: %s:%d", args_.hostname_.c_str(), args_.port_);
  return true;
}

bool MysqlHandler::InitSchema(const std::map<std::string, std::string>& table,
                              const std::map<std::string, std::string>& sp) {
  auto create_db = "create database if not exists " + args_.db_;
  if (mysql_query(conn_, create_db.c_str())) {
    LOG_F(ERROR, "Query failed with %s", create_db.c_str());
    LOG_F(ERROR, "Query failed with %s", mysql_error(conn_));
    return false;
  }

  if (mysql_select_db(conn_, args_.db_.c_str())) {
    LOG_F(ERROR, "Query failed with %s", args_.db_.c_str());
    LOG_F(ERROR, "Query failed with %s", mysql_error(conn_));
    return false;
  }

  for (const auto& [path, sql] : table) {
    // LOG_F(2, "create table file: %s", path.c_str());

    if (mysql_query(conn_, sql.c_str())) {
      LOG_F(ERROR, "Query failed with %s", sql.c_str());
      LOG_F(ERROR, "Query failed with %s", mysql_error(conn_));
      return false;
    }

    DBMRFREE(conn_);
  }

  return true;
}

bool MysqlHandler::QueryAsync(const std::string& sql) { return true; }

bool MysqlHandler::Query(MYSQL* conn, const std::string& sql) {
  if (mysql_query(conn, sql.c_str()) != 0) {
    LOG_F(ERROR, "Query failed with %s", sql.c_str());
    LOG_F(ERROR, "Query failed with %s", mysql_error(conn));
    return false;
  }

  DBMRFREE(conn);

  if (mysql_commit(conn) != 0) {
    LOG_F(ERROR, "Commit failed with %s", sql.c_str());
    LOG_F(ERROR, "Commit failed with %s", mysql_error(conn));
    return false;
  }

  return true;
}

bool MysqlHandler::Query(const std::string& sql) { return Query(conn_, sql); }

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

// 获取utf-8字符个数（utf-8下，英文字符一个站位一个字节，中文字符一个站位3个字节）
static int getUtf8LetterNumber(const char* s) {
  int i = 0, j = 0;
  while (s[i]) {
    if ((s[i] & 0xc0) != 0x80) j++;
    i++;
  }
  return j;
}

std::vector<std::vector<std::string>> MysqlHandler::QueryData(const std::string& sql) {
  std::vector<std::vector<std::string>> data;
  if (!DBResFree(conn_)) {
    return data;
  }

  if (mysql_query(conn_, sql.c_str()) != 0) {
    LOG_F(ERROR, "Commit failed with %s", mysql_error(conn_));
    return data;
  }

  auto res = mysql_use_result(conn_);
  auto num_fields = mysql_num_fields(res);
  auto fields = mysql_fetch_fields(res);

  std::vector<std::string> col_name;
  for (unsigned int i = 0; i < num_fields; i++) {
    auto real_len = getUtf8LetterNumber(fields[i].name);
    auto str_len = strlen(fields[i].name);
    auto padding_len = (str_len - real_len) / 2;
    auto right_padding = padding_len / 2;
    auto left_padding = padding_len - right_padding;
    std::string final_name = std::string(left_padding, ' ');
    final_name += fields[i].name;
    final_name += std::string(right_padding, ' ');
    LOG_F(INFO, "new name: %s.", final_name.c_str());
    col_name.emplace_back(final_name);
  }
  data.emplace_back(col_name);

  MYSQL_ROW row;

  while ((row = mysql_fetch_row(res)) != nullptr) {
    std::vector<std::string> row_data;
    for (unsigned int i = 0; i < num_fields; i++) {
      row_data.emplace_back(std::string(row[i]));
    }
    data.emplace_back(row_data);
    mysql_next_result(conn_);
  }

  return data;
}
