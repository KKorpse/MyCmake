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
  Configure(peloton_logging_mode, false, DEFAULT_NUM_FRONTEND_LOGGERS,
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


}  // namespace logging
}  // namespace fulgurDB
