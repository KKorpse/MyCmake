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

  //===--------------------------------------------------------------------===//
  // 主要函数
  //===--------------------------------------------------------------------===//

  // Log the given record
  void Log(LogRecord *record);


  //===--------------------------------------------------------------------===//
  // FlushLogger 相关
  //===--------------------------------------------------------------------===//

  // 获取所有已经写入记录的缓存块（包括当前未写满的缓存块）
  // 以引用的方式返回对应的max_commited_cid，min_cid，给 FlushLogger 使用
  // lower_bound_id 暂时不知道怎么使用
  std::vector<std::unique_ptr<LogBuffer>> &GetLogBuffers(txn_id_t &min_committed_id, txn_id_t &max_committed_id);

  // 回收使用后的LogBuffer，避免重新分配内存
  void GrantEmptyBuffer(std::unique_ptr<LogBuffer>);


  // LogManager 在收到 COMMIT 类型的记录时，调用 WaitForFlush()
  // FlushLogger 每批次刷新时， 在 GetLogBuffers() 中设置 max_flushed_commit_id 为 max_seen_txn_id（后者在log()中更新）
  // FlushLogger 结束刷新时，调用对应 BufferLogger 的 WaitForFlush()
  // 这样的设置理论上比让所有 BufferLogger 等待 global_max_cid 效率高一些
  void FlushLoggerFlushed();
  void WaitForFlush(txn_id_t cur_commit_txn_id);

  //===--------------------------------------------------------------------===//
  // Accecer
  //===--------------------------------------------------------------------===//

  // FlushLoggerID
  void SetFlushLoggerID(int id) { this->flush_logger_id = id; }
  int GetFlushLoggerID() { return this->flush_logger_id; }

  cid_t GetLowerBoundary() { return this->lower_boundary; }
  void SetLowerBoundary( cid_t lower_boundary) { this->lower_boundary = lower_boundary; }


  // 需要一个解决序号用完的方案
  // void SetLoggingCidLowerBound(cid_t cid) {
  //   logging_cid_lower_bound = cid;
  //   highest_logged_commit_message = INVALID_CID;
  // }

 private:
  // 在运输缓存池时使用，此时 BufferLogger 和 FlushLogger 分别是两个线程，需要互斥。
  Spinlock cur_log_buffer_lock;

  // 用于将 persiste_buffer_pool 中的数据运输给 FLushLogger
  std::vector<std::unique_ptr<LogBuffer>> prepared_buffers_for_flush;

  // 目前记录到的最大已经提交的记录的 txn_id
  cid_t max_commit_txn_id = INVALID_TXN_ID;
  cid_t max_seen_txn_id = INVALID_TXN_ID;

  // FlushLgger 绑定
  int Flush_logger_id = -1;

  //Buffer 相关
  std::unique_ptr<LogBuffer> cur_log_buffer_;   // the current buffer，当前 LogBuffer 块
  std::unique_ptr<BufferPool> available_buffer_pool_;
  std::unique_ptr<BufferPool> persist_buffer_pool_;

  // 等待刷新用的锁
  std::mutex flush_notify_mutex;
  std::condition_variable flush_notify_cv;
  cid_t max_flushed_commit_id = 0;

  cid_t max_seen_txn_id = 0;
  cid_t lower_boundary_cid = 0;
};

}  // namespace logging
}  // namespace fulgurDB
