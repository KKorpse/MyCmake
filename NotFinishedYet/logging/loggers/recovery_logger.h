

#pragma once

#include "logging/loggers/logger.h"
#include "logging/log_record.h"
#include "logging/log_file.h"
#include "executor/executors.h"

#include <dirent.h>
#include <vector>
#include <set>
#include <chrono>


namespace fulgurDB {

namespace logging {

//===--------------------------------------------------------------------===//
// Write Ahead Frontend Logger
//===--------------------------------------------------------------------===//

class Recovery_Logger : public FrontendLogger {
 public:
  Recovery_Logger(void);

  Recovery_Logger(bool for_testing);

  Recovery_Logger(std::string log_dir);

  ~Recovery_Logger(void);


  //===--------------------------------------------------------------------===//
  // Recovery
  //===--------------------------------------------------------------------===//

  void DoRecovery(void);

  void RecoverIndex();

  void StartTransactionRecovery(cid_t commit_id);

  void CommitTransactionRecovery(cid_t commit_id);

  void InsertTuple(TupleRecord *recovery_txn);

  void DeleteTuple(TupleRecord *recovery_txn);

  void UpdateTuple(TupleRecord *recovery_txn);

  void InitLogFilesList();

  void OpenNextLogFile();

  LogRecordType GetNextLogRecordTypeForRecovery();

  void TruncateLog(cid_t);

  std::string GetFileNameFromVersion(int);

  int GetLogFileCursor() { return log_file_cursor_; }

  int GetLogFileCounter() { return log_file_counter_; }

  void InitSelf();

  static constexpr auto wal_directory_path = "wal_log";

 private:
  std::string GetLogFileName(void);

  void InsertIndexEntry(storage::Tuple *tuple, storage::DataTable *table,
                        ItemPointer target_location);

  //===--------------------------------------------------------------------===//
  // Member Variables
  //===--------------------------------------------------------------------===//

  // File pointer and descriptor
  FileHandle cur_file_handle;

  // Txn table during recovery
  std::map<txn_id_t, std::vector<TupleRecord *>> recovery_txn_table;

  // Keep tracking max oid for setting next_oid in manager
  // For active processing after recovery
  oid_t max_oid = 0;

  cid_t max_cid = 0;

  // pool for allocating non-inlined values
  type::AbstractPool *recovery_pool;

  // abj1 adding code here!
  std::vector<LogFile *> log_files_;

  int log_file_counter_;

  int log_file_cursor_;

  int logger_id;

};

}  // namespace logging
}  // namespace fulgurDB
