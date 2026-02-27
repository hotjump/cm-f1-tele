#include "duckdb_handler.h"

#include <filesystem>
#include <string>

#include "common/log.h"

DuckDBHandler::~DuckDBHandler() {
  conn_.reset();
  db_instance_.reset();
}

bool DuckDBHandler::Init() {
  try {
    std::filesystem::create_directories(db_path_);
    db_instance_ =
        std::make_unique<duckdb::DuckDB>(db_path_ + "/" + db_ + ".duckdb");
    conn_ = std::make_unique<duckdb::Connection>(*db_instance_);
    return true;
  } catch (const std::exception& e) {
    LOG_ERROR("Failed to initialize DuckDB: {}", e.what());
    return false;
  }
}

bool DuckDBHandler::InitSchema(const std::map<std::string, std::string>& table,
                               const std::map<std::string, std::string>& sp) {
  LOG_INFO("DuckDBHandler::InitSchema");
  try {
    for (const auto& [table_name, table_sql] : table) {
      LOG_INFO("create table file: {}", table_name.c_str());
      auto res = conn_->Query(table_sql);
      if (res->HasError()) {
        LOG_ERROR("Failed to create table: {}", res->GetError());
        return false;
      }
    }

  # if 1
    // 查询所有用户表（schema = 'main' 表示默认 schema）
    auto result = conn_->Query(
        "SELECT table_name FROM information_schema.tables "
        "WHERE table_schema = 'main' "
        "ORDER BY table_name;");

    if (result->HasError()) {
      LOG_ERROR("Query error: {}", result->GetError());
      return false;
    }

    LOG_INFO("Tables in database");
    while (auto chunk = result->Fetch()) {
      for (idx_t row = 0; row < chunk->size(); row++) {
        duckdb::Value val = chunk->GetValue(0, row);
        if (!val.IsNull()) {
          LOG_INFO("Tables: {}", val.ToString());
        }
      }
    }
  # endif

    return true;
  } catch (const std::exception& e) {
    LOG_ERROR("Failed to initialize schema: {}", e.what());
    return false;
  }
}

bool DuckDBHandler::Query(std::string sql) {
  try {
    conn_->Query(sql);
    return true;
  } catch (const std::exception& e) {
    LOG_ERROR("Query failed: {}", e.what());
    return false;
  }
}
