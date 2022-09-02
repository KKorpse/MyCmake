
#pragma once

#include <map>
#include <mutex>
#include <vector>

#include "logging/type.h"
#include "logging/loggers/buffer_logger.h"
#include "logging/loggers/flush_logger.h"
// #include "logging/logger.h"
// #include "concurrency/transaction.h"

#define DEFAULT_NUM_Flush_LOGGERS 1
#define DEFAULT_NUM_RECOVERY_LOGGERS 1


namespace fulgurDB {
namespace logging {

thread_local static BackendLogger *backend_logger = nullptr;

// 删减了logBeginTransaction 和 LogCommitTransction，用传入的此参数判断
// 是否是Begin 或者 Commit，减少了logrecord的数量
enum RecordPosition {
  TRANSACTION_BEGIN = 1,

  TRANSACTION_COMMIT = 2,

  TRANSACTION_NORMAL = 3,
};


//===--------------------------------------------------------------------===//
// Log Manager
//===--------------------------------------------------------------------===//

#define LOG_FILE_LEN UINT64_C(128) * 1024 * 1024 // 128MB
#define LOG_BUFFER_LEN UINT64_C(128) * 1024 // 128KB
/**
 * Global Log Manager
 */
class LogManager {
 public:
  LogManager(const LogManager &) = delete;
  LogManager &operator=(const LogManager &) = delete;
  LogManager(LogManager &&) = delete;
  LogManager &operator=(LogManager &&) = delete;

  LogManager();
  ~LogManager();

  // global singleton
  static LogManager &GetInstance(void);

  // configuration
  void Configure(LoggingType logging_type, unsigned int num_Flush_loggers = 1) {
    logging_type_ = logging_type;
    num_Flush_loggers_ = num_Flush_loggers;
  }


  //===--------------------------------------------------------------------===//
  // 模式控制，转换
  //===--------------------------------------------------------------------===//

  // 等待模式转变为指定状态
  void WaitForModeTransition(LoggingStatusType logging_status);

  void SetLoggingStatus(LoggingStatusType logging_status);

  LoggingStatusType GetLoggingStatus();


  //===--------------------------------------------------------------------===//
  // Logging
  //===--------------------------------------------------------------------===//

  // 在 TimestampOrderingTransactionManager::BeginTransaction 中被调用
  // 负责为此线程分配 BufferLogger()
  void PrepareLogging();

  /*
  大致逻辑
  根据传入的参数构造 LogRecord (且不同的 RecordPosition 要正确设置对应的 RecordType)
  BufferLogger->Log(LogRecord)
  if (record_position == TRANSACTION_COMMIT)
    BufferLogger->WaitForFlush(commit_txn_id); // 解锁需要在 FlushLogger 中调用
  */
  void LogUpdate(txn_id_t txn_id, const ItemPointer &old_version, const ItemPointer &new_version,
                 RecordPosition record_position = RecordPosition::TRANSACTION_NORMAL);
  void LogInsert(txn_id_t txn_id, const ItemPointer &new_location,
                 RecordPosition record_position = RecordPosition::TRANSACTION_NORMAL);
  void LogDelete(txn_id_t txn_id, const ItemPointer &delete_location,
                 RecordPosition record_position = RecordPosition::TRANSACTION_NORMAL);

  // called if a transaction aborts before starting a commit
  void DoneLogging();

  //===--------------------------------------------------------------------===//
  // Recovery
  //===--------------------------------------------------------------------===//

  // 1. 调用 RecoveryFromCheckPoint，如果有检查点，会先载入检查点。
  // 2. 创建若干 RecoveryLogger，一个线程对应一个实例，从检查点开始恢复。
  void DoRecovery();

  // RecoveryLoggger 通过此函数通知 LogManager 完成恢复
  // LogManager 会阻塞直到所有 RecoveryLogger 完成恢复。
  void NotifyRecoveryDone();


  //===--------------------------------------------------------------------===//
  // CheckPoint
  //===--------------------------------------------------------------------===//

    // 调用 CheckPointLogger::DoRecovery()
  void RecoveryFromCheckPoint();



  //===--------------------------------------------------------------------===//
  // 功能性函数
  //===--------------------------------------------------------------------===//

  // 每个线程独有一个 BufferLogger 实例
  // 如果该线程有 BufferLogger，返回，没有则分配
  BackendLogger *GetBufferLogger(unsigned int hint = 0);

  // 根据定义的数量初始化 FlushLogger，并执行其MainLoop
  void InitFlushLoggers();

  // get a Flush logger at given index
  FlushLogger *GetFlushLogger(unsigned int logger_idx);


  //===--------------------------------------------------------------------===//
  // Accessors
  //===--------------------------------------------------------------------===//

  // 统一的日志文件抬头
  std::string GetLogFileNameHeader(void);
  void SetLogFileNameHeader(std::string log_file);

  // 统一的日志存储目录
  void SetLogDirectoryName(std::string log_dir);
  std::string GetLogDirectoryName(void);

 private:

  //===--------------------------------------------------------------------===//
  // Data members
  //===--------------------------------------------------------------------===//

  // LogManager 状态相关
  LoggingStatusType logging_status = LoggingStatusType::INVALID;
  std::mutex logging_status_mutex;
  std::condition_variable logging_status_cv;

  // FlushLogger 相关
  std::vector<std::unique_ptr<FlushLogger>> flush_loggers;
  unsigned int num_flush_loggers_ = DEFAULT_NUM_Flush_LOGGERS;
  int Flush_logger_assign_counter; // round robin counter for Flush logger assignment

  // RecoveryLogger 相关
  unsigned int finished_recovery_logger_counter = 0;
  unsigned int num_recovery_loggers_ = DEFAULT_NUM_RECOVERY_LOGGERS;
  

  // 文件大小 以及 Buffer 大小
  size_t log_file_size_limit_ = LOG_FILE_LEN; // 128MB
  size_t log_buffer_capacity_ = LOG_BUFFER_LEN; // 128KB

  // 日志记录的硬件类型
  LoggingType logging_type_ = LoggingType::INVALID;

  // 日志目录名
  std::string log_directory_name;
  // 日志文件名
  std::string log_file_name_header;

  // 恢复后反馈
  // oid_t max_oid_after_recovery = 0;
  // cid_t max_cid_after_recovery = 0;

};

}  // namespace logging
}  // namespace fulgurDB
