#include "logging/loggers/buffer_logger.h"
#include "logging/log_manager.h"
#include "logging/log_record.h"
// #include "logging/logging_util.h"

// for Debug
#include "logging/logger.h"

namespace furgurBD {
namespace logging {

BufferLogger::BufferLogger()
    : log_buffer_(std::unique_ptr<LogBuffer>(nullptr)),
      available_buffer_pool_(
          std::unique_ptr<BufferPool>(new CircularBufferPool())),
      persist_buffer_pool_(
          std::unique_ptr<BufferPool>(new CircularBufferPool())) {
  flush_logger_id = -1;
}


BufferLogger::~BufferLogger() {
  auto &log_manager = LogManager::GetInstance();
  // if (!shutdown && flush_logger_id != -1) {
  log_manager.GetFrontendLoggersList()[flush_logger_id]
      .get()
      ->RemoveBufferLogger(this);
}

// TODO
void BufferLogger::Log(LogRecord *record) {
  /*
  序列化 record
  if (cur_log_buffer_ == null) {
    从 persist_buffer_pool_ 获取一个空闲 buffer

  } else {
    if ( !cur_log_buffer_->WriteRecord(record) ) {
      从 persist_buffer_pool_ 获取一个空闲 buffer
      cur_log_buffer_->WriteRecord(record)
    }
  }

  更新 max_seen_txn_id 为当前 recored 的 id
  */
}

// TODO
std::vector<std::unique_ptr<LogBuffer>> &GetLogBuffers(txn_id_t &lower_bound_id, txn_id_t &max_commited_id) {
  // 一定要要跟 cur_log_buffer 一起锁，防止 max_commit_txn_id 在 Log()中被更新
  cur_log_buffer_lock.Lock();
  /*
  将 persist_buffer_pool 中的内容写入 prepared_buffers_for_flush
  if (!cur_log_buffer_.empty) {
    cur_log_buffer_ 中的内容也写入 prepared_buffers_for_flush
  }

  lower_bound_id = this->lower_boundary_cid;
  max_commited_id = this->max_seen_txn_id;

  this->max_flushed_commit_id = this->max_seen_txn_id;
  */
  cur_log_buffer_lock.Unlock();

  return prepared_buffers_for_flush;
}

// NEED TEST
void BufferLogger::GrantEmptyBuffer(std::unique_ptr<LogBuffer> empty_buffer) {
  available_buffer_pool_->Put(std::move(empty_buffer));
}

// NEED TEST
void BufferLogger::FlushLoggerFlushed() {
  {
    std::unique_lock<std::mutex> wait_lock(flush_notify_mutex);
    flush_notify_cv.notify_all();
  }
}

// NEED TEST
void BufferLogger::WaitForFlush(txn_id_t cur_commit_txn_id) {
  {
    std::unique_lock<std::mutex> wait_lock(flush_notify_mutex);
    while (this->max_flushed_commit_id < cur_commit_txn_id) {
      flush_notify_cv.wait(wait_lock);
    }
  }
}

} // logging
} // furgurBD