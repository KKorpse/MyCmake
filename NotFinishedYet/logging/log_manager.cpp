#include <condition_variable>
#include <memory>

// #include "catalog/catalog.h"
// #include "catalog/manager.h"
#include "logging/logger.h"
// #include "common/macros.h"
// #include "concurrency/transaction_manager_factory.h"
// #include "executor/executor_context.h"
#include "logging/log_manager.h"
// #include "logging/logging_util.h"
#include "logging/log_record.h"
// #include "storage/data_table.h"
// #include "storage/database.h"
// #include "storage/tile_group.h"
// #include "storage/tuple.h"

namespace fulgurDB {
namespace logging {

// Each thread gets a backend logger
thread_local static BackendLogger *backend_logger = nullptr;

LogManager::LogManager() {
  Configure(peloton_logging_mode, false, DEFAULT_NUM_flush_LOGGERS,
            LoggerMappingStrategyType::ROUND_ROBIN);
}

LogManager::~LogManager() {}

/**
 * @brief Return the singleton log manager instance
 */
LogManager &LogManager::GetInstance() {
  static LogManager log_manager;
  return log_manager;
}



void LogManager::PrepareLogging() {
  if (this->IsInLoggingMode()) {
    this->GetBufferLogger();
    auto logger = this->GetBufferLogger();
    int flush_logger_id = logger->GetFlushLoggerID();
    LOG_TRACE("Got flush_logger_id as %d", (int)flush_logger_id);
    flush_loggers[flush_logger_id]->SetBackendLoggerLoggedCid(*logger);
  }
}
/*
大致逻辑

根据传入的参数构造 LogRecord (且不同的 RecordPosition 要正确设置对应的 RecordType)
BufferLogger->Log(LogRecord)
if (record_position == TRANSACTION_COMMIT)
  BufferLogger->WaitForFlush(commit_txn_id); // 解锁需要在 FlushLogger 中调用

*/
// log an update
void LogManager::LogUpdate(txn_id_t txn_id, const ItemPointer &old_version, const ItemPointer &new_version,
                 RecordPosition record_position) {}

// log an insert
void LogManager::LogInsert(txn_id_t txn_id, const ItemPointer &new_location,
                 RecordPosition record_position) {}

// log a delete
void LogManager::LogDelete(txn_id_t txn_id, const ItemPointer &delete_location,
                 RecordPosition record_position) {}

// 为该线程获取 ThreadLocal 的 BufferLogger，默认轮询分配
BufferLogger *LogManager::GetBufferLogger(unsigned int hint_idx) {
  // 先检查此线程是否分配了 buffer_logger
  if (buffer_logger == nullptr) {
    buffer_logger = BufferLogger::GetBufferLogger(logging_type_);
    int i = __sync_fetch_and_add(&this->flush_logger_assign_counter, 1);

    // round robin mapping
    flush_loggers[i % num_flush_loggers_].get()->AddGetBufferLogger(
        backend_logger);
    buffer_logger->SetFlushLoggerID(i % num_flush_loggers_);
  }
  return buffer_logger;
}

// 初始化 FlushLogger
void LogManager::InitFlushLoggers() {
  if (this->flush_loggers.size() == 0) {
    for (unsigned int i = 0; i < this->num_flush_loggers_; i++) {
      std::unique_ptr<FlushLogger> flush_logger(logging_type_, test_mode_);
      // flush_logger->SetNoWrite(no_write_);

      if (flush_logger.get() != nullptr) {
        this->flush_loggers.push_back(std::move(flush_logger));
      }
    }
  }
}
}  // namespace logging
}  // namespace fulgurDB
