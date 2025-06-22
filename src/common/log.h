#pragma once

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <mutex>
#include <iostream>

class LoggerSingleton {
 public:
  // 获取单例实例
  static LoggerSingleton& getInstance(const std::string& log_file = "log/app.log") {
    static std::once_flag flag;
    static std::unique_ptr<LoggerSingleton> instance;

    std::call_once(flag, [&]() { instance.reset(new LoggerSingleton(log_file)); });

    return *instance;
  }

  // 删除拷贝构造函数和赋值运算符
  LoggerSingleton(const LoggerSingleton&) = delete;
  LoggerSingleton& operator=(const LoggerSingleton&) = delete;

  // 日志接口
  template <typename... Args>
  void trace(const char* fmt, Args&&... args) {
    if (logger_) logger_->trace(fmt, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void debug(const char* fmt, Args&&... args) {
    if (logger_) logger_->debug(fmt, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void info(const char* fmt, Args&&... args) {
    if (logger_) logger_->info(fmt, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void warn(const char* fmt, Args&&... args) {
    if (logger_) logger_->warn(fmt, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void error(const char* fmt, Args&&... args) {
    if (logger_) logger_->error(fmt, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void critical(const char* fmt, Args&&... args) {
    if (logger_) logger_->critical(fmt, std::forward<Args>(args)...);
  }

  // 设置日志级别
  void setLevel(spdlog::level::level_enum level) {
    if (logger_) logger_->set_level(level);
  }

  // 刷新日志
  void flush() {
    if (logger_) logger_->flush();
  }

  // 私有构造函数
  LoggerSingleton(const std::string& log_file) {
    try {
      // 创建基本文件 sink
      auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_file, true);

      // 创建日志器
      logger_ = std::make_shared<spdlog::logger>("basic_logger", file_sink);

      // 设置默认格式
      logger_->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");

      // 注册日志器
      spdlog::register_logger(logger_);

      // 设置默认日志级别
      logger_->set_level(spdlog::level::info);

      // 启用自动刷新（可选）
      spdlog::flush_every(std::chrono::seconds(3));

      logger_->info("Logger initialized: {}", log_file);
    } catch (const spdlog::spdlog_ex& ex) {
      std::cerr << "Log initialization failed: " << ex.what() << std::endl;
    }
  }

  // 析构函数
  ~LoggerSingleton() {
    if (logger_) {
      logger_->flush();
      spdlog::drop("basic_logger");
    }
  }

  std::shared_ptr<spdlog::logger> logger_;
};

// 使用宏简化调用
#define LOG_TRACE(...) LoggerSingleton::getInstance().trace(__VA_ARGS__)
#define LOG_DEBUG(...) LoggerSingleton::getInstance().debug(__VA_ARGS__)
#define LOG_INFO(...) LoggerSingleton::getInstance().info(__VA_ARGS__)
#define LOG_WARN(...) LoggerSingleton::getInstance().warn(__VA_ARGS__)
#define LOG_ERROR(...) LoggerSingleton::getInstance().error(__VA_ARGS__)
#define LOG_CRITICAL(...) LoggerSingleton::getInstance().critical(__VA_ARGS__)