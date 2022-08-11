

#include "logging/loggers/buffer_logger.h"
#include "logging/log_manager.h"
#include "logging/log_record.h"
// #include "logging/logging_util.h"

// for Debug
#include "logging/logger.h"

namespace peloton {
namespace logging {

// create a backend logger
BufferLogger::BufferLogger()
    : log_buffer_(std::unique_ptr<LogBuffer>(nullptr)),
      available_buffer_pool_(
          std::unique_ptr<BufferPool>(new CircularBufferPool())),
      persist_buffer_pool_(
          std::unique_ptr<BufferPool>(new CircularBufferPool())) {
  // backend_pool.reset(new type::EphemeralPool());
  frontend_logger_id = -1;
}

// destroy and cleanup
BufferLogger::~BufferLogger() {
  auto &log_manager = LogManager::GetInstance();
  if (!shutdown && frontend_logger_id != -1) {
    log_manager.GetFrontendLoggersList()[frontend_logger_id]
        .get()
        ->RemoveBufferLogger(this);
  }
}


/**
 * @brief log log a log record
 * @param log record
 */
void BufferLogger::Log(LogRecord *record) {}


std::vector<std::unique_ptr<LogBuffer>> &GetLogBuffers(txn_id_t &lower_bound_id, txn_id_t &max_commited_id) {
  cur_log_buffer_lock.Lock();
  // TODO : 将 persist_buffer_pool 以及 cur_log_buffer_ 中的内容写入 prepared_buffers_for_flush

  // 一定要要跟 cur_log_buffer 一起锁，防止 max_commit_txn_id 在 Log()中被更新
  this->max_flushed_commit_id = this->max_commit_txn_id
  cur_log_buffer_lock.Unlock();

  return prepared_buffers_for_flush;
}

// 给 DiskLogger 用于返还空闲的 LogBuffer
void BufferLogger::GrantEmptyBuffer(std::unique_ptr<LogBuffer> empty_buffer) {
  available_buffer_pool_->Put(std::move(empty_buffer));
}


void BufferLogger::FlushLoggerFlushed() {
  {
    // this->max_flushed_commit_id = 
    std::unique_lock<std::mutex> wait_lock(flush_notify_mutex);
    flush_notify_cv.notify_all();
  }
}

void BufferLogger::WaitForFlush(txn_id_t cur_commit_txn_id) {
  // LOG_TRACE("Waiting for flush with %d", (int)cid);
  {
    std::unique_lock<std::mutex> wait_lock(flush_notify_mutex);
    while (max_flushed_commit_id < cur_commit_txn_id) {
      // LOG_TRACE(
      //     "Logs up to %lu cid is flushed. %lu cid is not flushed yet. Wait...",
      //     this->GetPersistentFlushedCommitId(), cid);
      flush_notify_cv.wait(wait_lock);
    }
    // LOG_TRACE(
    //     "Flushes done! Can return! Got persistent flushed commit id as %d",
    //     (int)this->GetPersistentFlushedCommitId());
  }
}

// set when the frontend logger is shutting down, prevents deadlock between
// frontend and backend
// void BufferLogger::SetShutdown(bool val) { shutdown = val; }
// }  // namespace logging
}
