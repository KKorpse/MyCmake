#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <algorithm>
#include <dirent.h>
#include <numeric>

#include "logging/log_manager.h"
#include "logging/log_record.h"
#include "logging/loggers/logger.h"
#include "logging/logger.h"

// #include "catalog/catalog.h"
// #include "catalog/manager.h"
// #include "catalog/schema.h"
// #include "type/ephemeral_pool.h"
// #include "concurrency/transaction.h"
// #include "concurrency/transaction_manager_factory.h"
// #include "concurrency/transaction_manager.h"
// #include "logging/records/tuple_record.h"
// #include "logging/checkpoint_tile_scanner.h"
// #include "logging/logging_util.h"
// #include "logging/checkpoint_manager.h"
// #include "storage/database.h"
// #include "storage/data_table.h"
// #include "storage/tile_group.h"
// #include "storage/tuple.h"
// #include "index/index.h"
// #include "executor/executor_context.h"
// #include "planner/seq_scan_plan.h"

int logger_id_counter = 0;

//#define LOG_FILE_SWITCH_LIMIT (1024)

namespace furgurDB {
namespace logging {

FlushLogger::FlushLogger(unsigned int logger_id) {
  this->SetLoggerID(logger_id);
  InitLogDirectory();
  InitLogFilesList();
  cur_file_handle.fd = -1;
}

FlushLogger::~FlushLogger() { }

//===--------------------------------------------------------------------===//
// 主要函数
//===--------------------------------------------------------------------===//

// TODO
void FlushLogger::MainLoop(void) {
  /*
  while (log_manager.GetLoggingStatus() == LoggingStatusType::LOGGING) {
    CollectLogRecordsFromBufferLoggers();
    FlushLogRecords();
    // UpdateGlobalMaxFlushId();
    SleepForAWhile();
  }
  CollectLogRecordsFromBufferLoggers();
  FlushLogRecords();
  // log_manager.SetLoggingStatus(LoggingStatusType::SLEEP);
  */
}


//===--------------------------------------------------------------------===//
// 功能函数
//===--------------------------------------------------------------------===//

// TODO
void CollectLogRecordsFromBufferLoggers(void) {
  /*
  buffer_loggers_lock.lock();
  for (b : buffer_loggers) {
    B->persistent_buffer_pool 装入 flush_buffer_queue;
  }

  (按道理还需要控制 lower_bound 和 max_flushed_id 之类的界限，具体怎么控制？)
  */
}

// TODO
void FlushLogger::FlushLogRecords(void) { 

  /*

  for (i : )

  */

  // 放在末尾
  for (auto backend_logger : backend_loggers) {
    backend_logger->FlushLoggerFlushed();
  }
}

// NEED TEST
void FlushLogger::SleepForAWhile(void) {
  auto sleep_period = std::chrono::microseconds(wait_timeout);
  std::this_thread::sleep_for(sleep_period);
}

// TODO
void AddBufferLogger(BufferLogger *buffer_logger) {
  /*
  for (int i = 0; i < BUFFER_POOL_SIZE; i++) {
    std::unique_ptr<LogBuffer> buffer(new LogBuffer());
    buffer_logger->GrantEmptyBuffer(std::move(buffer));
  }
  buffer_loggers_lock.Lock();
  // backend_logger->SetLoggingCidLowerBound(max_collected_commit_id);
  buffer_loggers.push_back(buffer_logger);
  buffer_loggers_lock.Unlock();
  */
}


//===--------------------------------------------------------------------===//
// 文件相关
//===--------------------------------------------------------------------===//

// TODO
void CreateNewLogFile(bool) { }

// TODO
std::string CreateFileNameByVersionNumber(int version_number) { }

std::string GetLogFileNameHeaderFromManager(void) {
  auto &log_manager = logging::LogManager::GetInstance();
  return log_manager.GetLogFileNameHeader();
}

// TODO
bool NeedCreateNewFile() { return false; }

// TODO
void FlushLogger::InitLogDirectory() {
  auto &log_manager = logging::LogManager::GetInstance();
  furgurDB_log_directory = log_manager.GetLogDirectoryName();
  LoggingUtil::CreateDirectory(furgurDB_log_directory.c_str(), 0700);
}



}  // namespace logging
}  // namespace furgurDB
