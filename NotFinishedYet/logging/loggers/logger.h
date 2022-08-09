#pragma once

#include <string>

class Logger {
 public:
  Logger(void);

  ~Logger(void);

  void SetLoggerID(int);

 private:
  // 不同种类的 Logger id 可以冲突
  // FlushLogger 的 id 由 LogManager 设置，每个 FlushLogger 单独一个线程
  int logger_id;

  // 用于生成日志文件名，其中 directory 由 LogManager 设置
  /*
   文件名格式: fulgurDB_log_<version_number>_<logger_id>_.log
  */
  std::string fulgurDB_log_directory;
  std::string LOG_FILE_PREFIX = "fulgurDB_log_";
  std::string LOG_FILE_SUFFIX = ".log";
};
