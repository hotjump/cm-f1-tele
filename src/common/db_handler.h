#pragma once

#if __cplusplus >= 201703L && __has_include(<filesystem>)
#include <filesystem>
namespace fs = std::filesystem;
#elif __has_include(<experimental/filesystem>)
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#elif __has_include(<boost/filesystem.hpp>)
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#endif

#include <algorithm>
#include <condition_variable>
#include <fstream>
#include <functional>
#include <future>
#include <map>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "CircularBuffer/CircularBuffer.h"
#include "loguru/loguru.hpp"

template <typename T, typename DBArgs>
class DBHandler {
 public:
  DBHandler(size_t thread_num, DBArgs args) : thread_num_(thread_num), conn_(args), args_(args) {}
  ~DBHandler();

  bool Init();
  bool Query(const std::string& sql);
  bool QueryAsync(const std::string& sql);

 private:
  void ConsumeQueryThread(int idx, std::promise<bool>& promise_obj);

  CircularBuffer<std::string, 1024> sql_buffer_;
  const size_t thread_num_ = 2;
  std::mutex mutex_;
  std::condition_variable cv_;
  std::vector<std::thread> sql_query_threads_;
  bool running_ = true;

  T conn_;
  DBArgs args_;
  std::vector<std::shared_ptr<T>> async_conn_;
};

template <typename T>
std::string ThreadName(std::thread::id id) {
  std::string class_name = typeid(T).name();
  std::string class_name_part = class_name.substr(2, 6);
  std::stringstream ss;
  ss << id;
  std::string thread_id = ss.str();
  return class_name_part + "-" + thread_id.substr(thread_id.length() - 4);
}

template <typename T, typename DBArgs>
DBHandler<T, DBArgs>::~DBHandler() {
  running_ = false;
  cv_.notify_all();
  for (auto& one_thread : sql_query_threads_) {
    LOG_F(INFO, "%s join.", ThreadName<T>(one_thread.get_id()).c_str());
    one_thread.join();
  }
}

template <typename T, typename DBArgs>
bool DBHandler<T, DBArgs>::Init() {
  if (!conn_.Init()) {
    return true;
  }

  // read sql dir, and fetch create table statement and sp statement.
  std::map<std::string, std::string> create_table_file;
  std::map<std::string, std::string> create_procedure_file;
  for (const auto& path : fs::directory_iterator("sql")) {
    std::ifstream fin(path.path().string());
    std::stringstream buffer;
    buffer << fin.rdbuf();
    auto sql = buffer.str();
    auto file_name = path.path().string();

    if (file_name.substr(file_name.length() - 7, 7) != "_sp.sql") {
      create_table_file.emplace(file_name, sql);
    } else {
      create_procedure_file.emplace(file_name, sql);
    }
  }

  if (!conn_.InitSchema(create_table_file, create_procedure_file)) {
    return true;
  }

  for (size_t i = 0; i < thread_num_; i++) {
    std::promise<bool> promise_conncct_success;
    std::future<bool> future_conncct_success = promise_conncct_success.get_future();
    auto sql_query_thread =
        std::bind(&DBHandler::ConsumeQueryThread, this, std::placeholders::_1, std::placeholders::_2);
    sql_query_threads_.emplace_back(std::thread(sql_query_thread, i, std::ref(promise_conncct_success)));
    if (future_conncct_success.get() == false) {
      return false;
    }
  }
  return true;
}

template <typename T, typename DBArgs>
bool DBHandler<T, DBArgs>::Query(const std::string& sql) {
  return Query(conn_, sql);
}

template <typename T, typename DBArgs>
bool DBHandler<T, DBArgs>::QueryAsync(const std::string& sql) {
  std::lock_guard<std::mutex> lock(mutex_);
  if (!sql_buffer_.isFull()) {
    sql_buffer_.push(sql);
    cv_.notify_one();
    return true;
  } else {
    return false;
  }
}

template <typename T, typename DBArgs>
void DBHandler<T, DBArgs>::ConsumeQueryThread(int idx, std::promise<bool>& promise_obj) {
  std::string thread_name = ThreadName<T>(std::this_thread::get_id());
  loguru::set_thread_name(thread_name.c_str());
  LOG_F(INFO, "%s enter.", thread_name.c_str());

  T conn(args_);
  if (conn.Init()) {
    promise_obj.set_value(true);
  } else {
    promise_obj.set_value(false);
  }

  while (running_) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (sql_buffer_.isEmpty()) {
      auto status = cv_.wait_for(lock, std::chrono::seconds(60));
      if (status == std::cv_status::timeout) {
        LOG_SCOPE_F(1, "mysql ping");
        conn_.Ping();
      }
    }
    if (!sql_buffer_.isEmpty()) {
      LOG_SCOPE_F(1, "mysql query");
      std::string sql = std::move(sql_buffer_.pop());
      lock.unlock();
      conn_.Query(sql);
    }
  }

  // LOG_F(INFO, "%s leave and close connection to database: %s:%d", thread_name, host_.c_str(), port_);
}