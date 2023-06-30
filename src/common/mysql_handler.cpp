#include "mysql_handler.h"
#include "loguru/loguru.hpp"

#include <functional>

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

MysqlHandler::~MysqlHandler() {
  if (conn_) {
    mysql_close(conn_);
    LOG_F(INFO, "close connection to database: %s:%d", host_.c_str(), port_);
  }

  running_ = false;
  cv_.notify_all();
  for (auto& one_thread : sql_query_threads_) {
    one_thread.join();
  }
}

bool MysqlHandler::InitMysqlConn(MYSQL*& conn, const std::string& host, const std::string& user,
                                 const std::string& passwd, const std::string& db, const int& port,
                                 const char* socket_name) {
  if ((conn = mysql_init(NULL)) == NULL) {
    LOG_F(ERROR, "init mysql client failed");
    return false;
  }

  bool reconnect = true;
  mysql_options(conn, MYSQL_OPT_RECONNECT, &reconnect);

  if (mysql_real_connect(conn, host.c_str(), user.c_str(), passwd.c_str(), db.c_str(), port, socket_name,
                         CLIENT_MULTI_STATEMENTS) == NULL) {
    mysql_close(conn);
    LOG_F(ERROR, "mysql_real_connect failed: %s", mysql_error(conn));
    return false;
  }

  if (mysql_set_character_set(conn, "utf8")) {
    mysql_close(conn);
    LOG_F(ERROR, "mysql_set_character_set failed");
    return false;
  }

  if (mysql_autocommit(conn, 0) != 0) {
    mysql_close(conn);
    LOG_F(ERROR, "mysql_autocommit(conn_, 0) failed");
    return false;
  }

  LOG_F(INFO, "connection to database: %s:%d", host.c_str(), port);
  return true;
}

bool MysqlHandler::Init() {
  if (!MysqlHandler::InitMysqlConn(conn_, host_, user_, passwd_, db_, port_, socket_name_)) {
    return false;
  }

  for (size_t i = 0; i < thread_num_; i++) {
    std::promise<bool> promise_conncct_success;
    std::future<bool> future_conncct_success = promise_conncct_success.get_future();
    auto sql_query_thread =
        std::bind(&MysqlHandler::ConsumeQueryThread, this, std::placeholders::_1, std::placeholders::_2);
    sql_query_threads_.emplace_back(std::thread(sql_query_thread, i, std::ref(promise_conncct_success)));
    if (future_conncct_success.get() == false) {
      return false;
    }
  }

  return true;
}

void MysqlHandler::ConsumeQueryThread(int idx, std::promise<bool>& promise_obj) {
  char thread_name[16];
  snprintf(thread_name, sizeof(thread_name), "sql-query-%d", idx);
  loguru::set_thread_name(thread_name);
  LOG_F(INFO, "%s enter.", thread_name);

  MYSQL* conn = nullptr;
  if (!MysqlHandler::InitMysqlConn(conn, host_, user_, passwd_, db_, port_, socket_name_)) {
    promise_obj.set_value(false);
    return;
  } else {
    promise_obj.set_value(true);
  }

  while (running_) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (sql_buffer_.isEmpty()) {
      auto status = cv_.wait_for(lock, std::chrono::seconds(60));
      if (status == std::cv_status::timeout) {
        LOG_SCOPE_F(1, "mysql ping");
        mysql_ping(conn);
      }
    }
    if (!sql_buffer_.isEmpty()) {
      LOG_SCOPE_F(1, "mysql query");
      std::string sql = std::move(sql_buffer_.pop());
      lock.unlock();
      Query(conn, sql);
    }
  }

  mysql_close(conn);
  LOG_F(INFO, "%s leave and close connection to database: %s:%d", thread_name, host_.c_str(), port_);
}

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

bool MysqlHandler::QueryAsync(const std::string& sql) {
  std::lock_guard<std::mutex> lock(mutex_);
  if (!sql_buffer_.isFull()) {
    sql_buffer_.push(sql);
    cv_.notify_one();
    return true;
  } else {
    return false;
  }
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
