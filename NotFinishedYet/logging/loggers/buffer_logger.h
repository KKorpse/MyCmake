
#pragma once

#include <condition_variable>
#include <mutex>
#include <vector>

#include "logging/otherone/item_pointer.h"
// #include "common/platform.h"
#include "logging/bufferpool/circular_buffer_pool.h"
#include "logging/bufferpool/log_buffer.h"
#include "logging/log_record.h"
#include "logging/loggers/logger.h"
#include "logging/types.h"

// for Debug
#include "logging/logger.h"

namespace fulgurDB {
namespace logging {

//===--------------------------------------------------------------------===//
// Buffer Logger
//===--------------------------------------------------------------------===//

class BufferLogger : public Logger {
  friend class FlushLogger;

 public:
  BufferLogger();

  ~BufferLogger();

  // Log the given record
  void Log(LogRecord *record);

  // 获取所有已经写入记录的缓存块（包括当前未写满的缓存块）
  // 以引用的方式返回对应的max_commited_cid，min_cid，给 FlushLogger 使用
  // std::vector<std::unique_ptr<LogBuffer>> &GetPreparedLogBuffers() { return prepared_buffers_for_flush; }
  // std::pair<cid_t, cid_t> PrepareLogBuffers();
  std::vector<std::unique_ptr<LogBuffer>> &GetLogBuffers(txn_id_t &lower_bound_id, txn_id_t &max_commited_id);


  // Grant an empty buffer to use
  void GrantEmptyBuffer(std::unique_ptr<LogBuffer>);

  // Set FrontendLoggerID
  void SetFrontendLoggerID(int id) { frontend_logger_id = id; }

  // Get FrontendLoggerID
  int GetFrontendLoggerID() { return frontend_logger_id; }


    // method for frontend to inform waiting backends of a flush to disk
  void FlushLoggerFlushed();
  // wait for the flush of a frontend logger (for worker thread)
  void WaitForFlush(txn_id_t cur_commit_txn_id);

  // set when the frontend is dying
  // void SetShutdown(bool);

  // gets the Varlenpool used for log serialization
  // type::AbstractPool *GetVarlenPool() { return backend_pool.get(); }

 protected:
  // 在运输缓存池时使用，此时 BufferLogger 和 FlushLogger 分别是两个线程，需要互斥。
  Spinlock cur_log_buffer_lock;

  // 用于将 persiste_buffer_pool 中的数据运输给 FLushLogger
  std::vector<std::unique_ptr<LogBuffer>> prepared_buffers_for_flush;

  // 目前记录到的最大已经提交的记录的 txn_id
  cid_t max_commit_txn_id = INVALID_TXN_ID;
  cid_t max_seen_txn_id = INVALID_TXN_ID;

  // id of the corresponding frontend logger
  int frontend_logger_id = -1;  // default

  // the current buffer
  std::unique_ptr<LogBuffer> cur_log_buffer_;
  std::unique_ptr<BufferPool> available_buffer_pool_;
  std::unique_ptr<BufferPool> persist_buffer_pool_;

    // To wait for flush
  std::mutex flush_notify_mutex;
  std::condition_variable flush_notify_cv;
  cid_t max_flushed_commit_id = 0;

  // max cid for the current log buffer
  // cid_t max_log_id_buffer = 0;

  // temporary serialization buffer
  // CopySerializeOutput output_buffer;

  // lower bound for values this backend may commit
  // cid_t logging_cid_lower_bound = INVALID_CID;

  // varlen pool for serialization
  // std::unique_ptr<type::AbstractPool> backend_pool;

  // shutdown flag
  // bool shutdown = false;
};

}  // namespace logging
}  // namespace fulgurDB
