#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <algorithm>
#include <dirent.h>
#include <numeric>

// #include "catalog/catalog.h"
// #include "catalog/manager.h"
// #include "catalog/schema.h"
// #include "type/ephemeral_pool.h"
// #include "concurrency/transaction.h"
// #include "concurrency/transaction_manager_factory.h"
// #include "concurrency/transaction_manager.h"

#include "logging/log_manager.h"
#include "logging/log_record.h"
// #include "logging/records/tuple_record.h"
// #include "logging/loggers/wal_frontend_logger.h"
#include "logging/loggers/logger.h"
// #include "logging/checkpoint_tile_scanner.h"
// #include "logging/logging_util.h"
// #include "logging/checkpoint_manager.h"

// #include "storage/database.h"
// #include "storage/data_table.h"
// #include "storage/tile_group.h"
// #include "storage/tuple.h"
#include "logging/logger.h"
// #include "index/index.h"
// #include "executor/executor_context.h"
// #include "planner/seq_scan_plan.h"

int logger_id_counter = 0;

//#define LOG_FILE_SWITCH_LIMIT (1024)

namespace furgurDB {
namespace logging {

FlushLogger::FlushLogger() {
  // logger_type = LoggerType::FRONTEND;

  // // Set wait timeout
  // wait_timeout = furgurDB_wait_timeout;
  // // LOG_TRACE("Init frontend logger with wait_time: %d ", (int)wait_timeout);

  // test_mode_ = for_testing;

  // // allocate pool
  // recovery_pool = new type::EphemeralPool();
  // if (test_mode_) {
  //   cur_file_handle.file = nullptr;
  // } else {
  //   InitSelf();
  // }
}

FlushLogger::FlushLogger(std::string log_dir)
    : furgurDB_log_directory(log_dir) {
}

FlushLogger::~FlushLogger() {
  backend_loggers_lock.Lock();
  for (auto backend_logger : backend_loggers) {
    backend_logger->SetShutdown(true);
    delete backend_logger;
  }
  backend_loggers_lock.Unlock();
}

void FlushLogger::InitSelf() {
  SetLoggerID(__sync_fetch_and_add(&logger_id_counter, 1));
  InitLogDirectory();
  InitLogFilesList();
  // UpdateMaxDelimiterForRecovery();
  // LOG_TRACE("Updated Max Delimiter for Recovery as %d",
  //           (int)max_delimiter_for_recovery);
  // cur_file_handle.fd = -1;  // this is a restart or a new start
  // max_log_id_file = 0;      // 0 is unused
}

/**
 * @brief close logfile
 */
FlushLogger::~FlushLogger() {
  // // close the log file
  // if (cur_file_handle.file != nullptr) {
  //   int ret = fclose(cur_file_handle.file);
  // }

  // for (auto log_file : log_files_) delete log_file;
  // // clean up pool
  // delete recovery_pool;
}

/**
 * @brief MainLoop
 */
void FrontendLogger::MainLoop(void) {
  auto &log_manager = LogManager::GetInstance();

  /////////////////////////////////////////////////////////////////////
  // STANDBY MODE
  /////////////////////////////////////////////////////////////////////

  LOG_TRACE("FrontendLogger Standby Mode");

  // Standby before we need to do RECOVERY
  log_manager.WaitForModeTransition(LoggingStatusType::STANDBY, false);

  // Do recovery if we can, otherwise terminate
  switch (log_manager.GetLoggingStatus()) {
    case LoggingStatusType::RECOVERY: {
      LOG_TRACE("Frontendlogger Recovery Mode");

      /////////////////////////////////////////////////////////////////////
      // RECOVERY MODE
      /////////////////////////////////////////////////////////////////////

      // First, do recovery if needed
      LOG_TRACE("Invoking DoRecovery");
      DoRecovery();
      LOG_TRACE("DoRecovery done");

      // Now, enter LOGGING mode
      // log_manager.SetLoggingStatus(LoggingStatusType::LOGGING);
      // Notify log manager that this frontend logger has completed recovery
      log_manager.NotifyRecoveryDone();

      // Now wait until the other frontend loggers also complete their recovery
      log_manager.WaitForModeTransition(LoggingStatusType::RECOVERY, false);

      break;
    }

    case LoggingStatusType::LOGGING: {
      LOG_TRACE("Frontendlogger Logging Mode");
    } break;

    default:
      break;
  }

  /////////////////////////////////////////////////////////////////////
  // LOGGING MODE
  /////////////////////////////////////////////////////////////////////

  // Periodically, wake up and do logging
  while (log_manager.GetLoggingStatus() == LoggingStatusType::LOGGING) {
    // Collect LogRecords from all backend loggers
    // LOG_TRACE("Log manager: Invoking CollectLogRecordsFromBackendLoggers");
    CollectLogRecordsFromBackendLoggers();

    // Flush the data to the file
    // LOG_TRACE("Log manager: Invoking FlushLogRecords");
    FlushLogRecords();

    // update the global max flushed ID (only distinguished logger does this)
    UpdateGlobalMaxFlushId();
  }

  /////////////////////////////////////////////////////////////////////
  // TERMINATE MODE
  /////////////////////////////////////////////////////////////////////

  // flush any remaining log records
  CollectLogRecordsFromBackendLoggers();
  FlushLogRecords();

  /////////////////////////////////////////////////////////////////////
  // SLEEP MODE
  /////////////////////////////////////////////////////////////////////

  LOG_TRACE("Frontendlogger Sleep Mode");

  // Setting frontend logger status to sleep
  log_manager.SetLoggingStatus(LoggingStatusType::SLEEP);
}

/**
 * @brief flush all the log records to the file
 */
void FlushLogger::FlushLogRecords(void) { 
  // 开头（这个函数有必要单独作为一个功能拆分出去么）
  CollectLogRecordsFromBackendLoggers();

  // 放在末尾
  for (auto backend_logger : backend_loggers) {
    backend_logger->FlushLoggerFlushed();
  }

}



//===--------------------------------------------------------------------===//
// Utility functions
//===--------------------------------------------------------------------===//

LogRecordType FlushLogger::GetNextLogRecordTypeForRecovery() {
  return log_record_type;
}

std::string FlushLogger::GetLogFileName(void) {
  // auto &log_manager = logging::LogManager::GetInstance();
  // return log_manager.GetLogFileName();
}


void FlushLogger::CreateNewLogFile(bool close_old_file) { }

/////////////////////
bool FlushLogger::FileSwitchCondIsTrue() { }

void FlushLogger::InitLogDirectory() {
  // Get log directory
  auto &log_manager = logging::LogManager::GetInstance();
  furgurDB_log_directory =
      log_manager.GetLogDirectoryName() + wal_directory_path;

  auto success =
      LoggingUtil::CreateDirectory(furgurDB_log_directory.c_str(), 0700);
  if (success) {
    LOG_TRACE("Logging directory is: %s", furgurDB_log_directory.c_str());
  } else {
    LOG_ERROR("Failed to create logging directory");
  }
}

std::string FlushLogger::GetFileNameFromVersion(int version) {
  return std::string(furgurDB_log_directory.c_str()) + "/" + LOG_FILE_PREFIX +
         std::to_string(version) + LOG_FILE_SUFFIX;
}


void FlushLogger::SetLoggerID(int id) {
  logger_id = id;
  furgurDB_log_directory += std::to_string(id);
}


}  // namespace logging
}  // namespace furgurDB
