#include "sqlite_handler.h"

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

#include <spdlog/spdlog.h>

#include "common/log.h"

SqliteHandler::~SqliteHandler() {
  if (conn_) {
    sqlite3_close(conn_);
  }
}

bool SqliteHandler::Init() {
  if (!fs::create_directory(db_path_)) {
    LOG_INFO("create_directory failed, maybe existed: {}", db_path_.c_str());
  }

  auto rc =
      sqlite3_open_v2((db_path_ + "/" + db_).c_str(), &conn_,
                      SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_SHAREDCACHE, NULL);
  if (rc) {
    LOG_ERROR("init sqlite3 failed: {}", sqlite3_errmsg(conn_));
    sqlite3_close(conn_);
    conn_ = nullptr;
    return false;
  }

  return true;
}

bool SqliteHandler::InitSchema(const std::map<std::string, std::string>& table,
                               const std::map<std::string, std::string>& sp) {
  for (const auto& [path, sql] : table) {
    // // LOG_F(2, "create table file: %s", path.c_str());

    auto rc = sqlite3_exec(conn_, sql.c_str(), NULL, NULL, NULL);
    if (rc) {
      LOG_ERROR("sqlite3_exec failed: {}", sqlite3_errmsg(conn_));
      sqlite3_close(conn_);
      conn_ = nullptr;
      return false;
    }
  }

  //for (const auto& [path, sql] : sp) {
    // LOG_INFO("create procedure file: %s", path.c_str());
  //}

  return true;
}

bool SqliteHandler::Query(std::string sql) {
  auto rc = sqlite3_exec(conn_, sql.c_str(), NULL, NULL, NULL);
  if (rc) {
    LOG_ERROR("sqlite3_exec failed: {}", sqlite3_errmsg(conn_));
    return false;
  }

  return true;
}