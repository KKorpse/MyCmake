
#pragma once

#include "logging/loggers/logger.h"
#include "logging/log_record.h"
#include "logging/log_file.h"
#include "logging/type.h"

#include <dirent.h>
#include <vector>
#include <set>
#include <chrono>

extern int fulgurDB_flush_frequency_micros;

namespace fulgurDB {

namespace logging {

typedef std::chrono::high_resolution_clock Clock;

typedef std::chrono::microseconds Micros;

typedef std::chrono::time_point<Clock> TimePoint;
//===--------------------------------------------------------------------===//
// Write Ahead Frontend Logger
//===--------------------------------------------------------------------===//

class FlushLogger : public FrontendLogger {
 public:
  FlushLogger(void);

  FlushLogger(bool for_testing);

  FlushLogger(std::string log_dir);

  ~FlushLogger(void);


  void MainLoop(void);

  void FlushLogRecords(void);

  void AddBackendLogger(BackendLogger *backend_logger);

  void SetFlushLoggerID(int);

  void InitSelf();

  static constexpr auto wal_directory_path = "wal_log";

 private:

  //===--------------------------------------------------------------------===//
  // Utilize Functions
  //===--------------------------------------------------------------------===//
  void CreateNewLogFile(bool);

  std::string GetLogFileName(void);

  void CollectLogRecordsFromBackendLoggers(void);

  bool FileSwitchCondIsTrue();

  void InitLogDirectory();
  //===--------------------------------------------------------------------===//
  // Member Variables
  //===--------------------------------------------------------------------===//
  

  // Associated backend loggers
  std::vector<BackendLogger *> backend_loggers;

  // File pointer and descriptor
  FileHandle cur_file_handle;


  cid_t max_flushed_commit_id = 0;

  cid_t max_collected_commit_id = 0;

  // 等待时间（前提是按时间间隔刷新）
  int wait_timeout;


  int log_file_counter_;

  int log_file_cursor_;

  std::string fulgurDB_log_directory;

  cid_t max_log_id_file = INVALID_CID;


  bool should_create_new_file = false;

  TimePoint last_flush = Clock::now();

  Micros flush_frequency{fulgurDB_flush_frequency_micros};
};

}  // namespace logging
}  // namespace fulgurDB
