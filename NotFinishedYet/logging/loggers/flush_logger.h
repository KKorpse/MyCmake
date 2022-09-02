
#pragma once

#include "logging/loggers/logger.h"
#include "logging/log_record.h"
#include "logging/log_file.h"
#include "logging/type.h"

#include <dirent.h>
#include <vector>
#include <set>
#include <chrono>

// extern int fulgurDB_flush_frequency_micros;

namespace fulgurDB {

namespace logging {

// typedef std::chrono::high_resolution_clock Clock;

// typedef std::chrono::microseconds Micros;

// typedef std::chrono::time_point<Clock> TimePoint;


class FlushLogger : public Logger {
 public:
  FlushLogger(void);
  FlushLogger(unsigned int logger_id);
  ~FlushLogger(void);

  //===--------------------------------------------------------------------===//
  // 主要函数
  //===--------------------------------------------------------------------===//

  void MainLoop(void);

  //===--------------------------------------------------------------------===//
  // 功能函数
  //===--------------------------------------------------------------------===//

  // 1.先收集所有 BufferLogger 的数据
  //（需要更新 ）
  void CollectLogRecordsFromBufferLoggers(void);

  // 2.再将其刷入磁盘
  void FlushLogRecords(void);

  // 休眠一段时间，长度为 wait_timeout
  void SleepForAWhile(void);

  // 添加其附属的 BufferLogger
  void AddBufferLogger(BufferLogger *buffer_logger);

  //===--------------------------------------------------------------------===//
  // 文件相关
  //===--------------------------------------------------------------------===//

  // 当前文件满时，创建新文件
  void CreateNewLogFile(bool);

  // 根据版本号创建一个新的文件名字，在 CreateNewLogFile() 中使用
  // 版本号其实就是文件的数量：log_file_counter_
  std::string CreateFileNameByVersionNumber(int version);

  // 从 LogManger 获取统一的日志文件抬头
  std::string GetLogFileNameHeaderFromManager(void);

  // 是否满足创建新文件的条件 （当前文件还有没有空闲空间）
  bool NeedCreateNewFile();

  // 根据 directory_path 初始化日志文件目录
  void InitLogDirectory();

 private:

  static constexpr auto directory_path = "log";
  
  Spinlock buffer_loggers_lock;
  std::vector<BufferLogger *> buffer_loggers;

  // 用于装载从 BufferLogger 中收集到的 Buffer，并刷新到磁盘
  std::vector<std::unique_ptr<LogBuffer>> flush_buffer_queue; 

  // 当前文件句柄
  FileHandle cur_file_handle;

  // 用于记录入每个文件的抬头
  cid_t max_flushed_commit_id = 0;
  // cid_t max_collected_commit_id = 0;

  // 等待时间（前提是按时间间隔刷新）
  int wait_timeout;

  // 文件数量，也用做 version_numebr
  int log_file_counter_;

  // 需要创建新的文件
  // bool should_create_new_file = false;
  // int log_file_cursor_;
  // TimePoint last_flush = Clock::now();
  // Micros flush_frequency{fulgurDB_flush_frequency_micros};
  // std::string furgurDB_log_directory;
};

}  // namespace logging
}  // namespace fulgurDB
