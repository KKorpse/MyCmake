#include <condition_variable>
#include <memory>

#include "logging/logger.h"
#include "logging/log_record.h"
#include "logging/log_manager.h"

// #include "catalog/catalog.h"
// #include "catalog/manager.h"
// #include "common/macros.h"
// #include "concurrency/transaction_manager_factory.h"
// #include "executor/executor_context.h"
// #include "logging/logging_util.h"
// #include "storage/data_table.h"
// #include "storage/database.h"
// #include "storage/tile_group.h"
// #include "storage/tuple.h"

namespace fulgurDB {
namespace logging {

// Each thread gets a Buffer logger
thread_local static BufferLogger *buffer_logger = nullptr;

LogManager::LogManager() {
  Configure(peloton_logging_mode, false, DEFAULT_NUM_flush_LOGGERS,
            LoggerMappingStrategyType::ROUND_ROBIN);
}

LogManager::~LogManager() {}


LogManager &LogManager::GetInstance() {
  static LogManager log_manager;
  return log_manager;
}



//===--------------------------------------------------------------------===//
// 模式控制，转换
//===--------------------------------------------------------------------===//

// NEED TEST
void LogManager::WaitForModeTransition(LoggingStatusType logging_status) {
  {
    std::unique_lock<std::mutex> wait_lock(this->logging_status_mutex);

    while (logging_status != this->logging_status) {
      this->logging_status_cv.wait(wait_lock);
    }
  }
}

// NEED TEST
void LogManager::SetLoggingStatus(LoggingStatusType logging_status) {
  {
    std::lock_guard<std::mutex> wait_lock(this->logging_status_mutex);

    this->logging_status = logging_status;

    this->logging_status_cv.notify_all();
  }
}

// NEED TEST
LoggingStatusType LogManager::GetLoggingStatus() {
  return this->logging_status;
}


//===--------------------------------------------------------------------===//
// Recovery
//===--------------------------------------------------------------------===//

// TODO
void LogManager::PrepareLogging() {
  /*
  if 若该线程有 BufferLogger {
    continue

  } else {
    创建一个新的 BufferLogger
    将其绑定到指定的 FlushLogger（轮询法）
  }

  根据其对应 FlushLogger 的 max_seen_cid 更新此 BufferLogger 的 LowerBoundary（下边界）
  */
}

// TODO
void LogManager::LogUpdate(txn_id_t txn_id, const ItemPointer &old_version, const ItemPointer &new_version, RecordPosition record_position) {
  /*
  swich (record_position) {
    case TRANSACTION_NORMAL: {
      log_record_type = LogRecordType::LOGRECORD_TYPE_INSERT;
    }
    case TRANSACTION_BEGIN {
      log_record_type = LogRecordType::LOGRECORD_TYPE_INSERT_BEGIN;
    }
    case TRANSACTION_COMMIT {
    log_record_type = LogRecordType::LOGRECORD_TYPE_INSERT_COMMIT;
    }
  }

  根据传入的参数以及 log_record_type, 创建一个 LogRecord（包含header，body）
  BufferLogger->Log(LogRecord);

  if (record_position == TRANSACTION_COMMIT) { // commit 类型的记录需要等待刷新到磁盘
    BufferLogger->WaitForFlush(commit_txn_id); // 解锁需要在 FlushLogger 中调用
  }
  */
}
// TODO
void LogManager::LogInsert(txn_id_t txn_id, const ItemPointer &new_location, RecordPosition record_position) {}
// TODO
void LogManager::LogDelete(txn_id_t txn_id, const ItemPointer &delete_location, RecordPosition record_position) {}

// TODO
// void LogManager::DoneLogging() { }


//===--------------------------------------------------------------------===//
// Recovery
//===--------------------------------------------------------------------===//

// TODO
void LogManager::DoRecovery() {
  /*
  调用 RecoveryFromCheckPoint，如果有检查点，会先载入检查点。
  for ( i in num_recovery_loggers_ ) {
    创建独立线程的 RecoveryLogger 实例并执行恢复。
  }
  while (finished_recovery_logger_counter < num_recovery_loggers_) {
    等待
  }
  */
  }

// TODO
void LogManager::NotifyRecoveryDone() {
  /*
  互斥对 finished_recovery_logger_counter 进行加一
  */
}


//===--------------------------------------------------------------------===//
// CheckPoint
//===--------------------------------------------------------------------===//

// TODO
void LogManager::RecoveryFromCheckPoint() {
    /*
  */
}



//===--------------------------------------------------------------------===//
// 功能性函数
//===--------------------------------------------------------------------===//

// NEED TEST
BufferLogger *LogManager::GetBufferLogger(unsigned int hint_idx) {
  // 先检查此线程是否分配了 buffer_logger
  if (buffer_logger == nullptr) {
    buffer_logger = BufferLogger::GetBufferLogger(logging_type_);
    int i = __sync_fetch_and_add(&this->flush_logger_assign_counter, 1);
    this->flush_loggers[i % this->num_flush_loggers_].get()->AddGetBufferLogger(
        buffer_logger);
    buffer_logger->SetFlushLoggerID(i % this->num_flush_loggers_);
  }
  return buffer_logger;
}

// NEED TEST
void LogManager::InitFlushLoggers() {
  if (this->flush_loggers.size() == 0) {
    for (unsigned int i = 0; i < this->num_flush_loggers_; i++) {
      std::unique_ptr<FlushLogger> flush_logger(i);

      if (flush_logger.get() != nullptr) {
        this->flush_loggers.push_back(std::move(flush_logger));
      }
    }
  }
}

// TODO
FlushLogger *LogManager::GetFlushLogger(unsigned int logger_idx) { 
  /*
  return this->flush_loggers[logger_idx];
  */
}


//===--------------------------------------------------------------------===//
// Accessors
//===--------------------------------------------------------------------===//

void LogManager::SetLogFileNameHeader(std::string log_file_name_header) {
  this->log_file_name_header = log_file_name_header;
}

std::string GetLogFileNameHeader(void) {
  return this->log_file_name_header;
}

void LogManager::SetLogDirectoryName(std::string log_directory_name) {
  this->log_directory_name = log_directory_name;
}

std::string LogManager::GetLogDirectoryName(void) {
  return this->log_directory_name;
}

}  // namespace logging
}  // namespace fulgurDB
