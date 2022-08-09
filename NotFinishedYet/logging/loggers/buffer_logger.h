
#pragma once

#include <condition_variable>
#include <mutex>
#include <vector>

#include "common/item_pointer.h"
#include "common/platform.h"
#include "logging/circular_buffer_pool.h"
#include "logging/log_buffer.h"
#include "logging/log_record.h"
#include "logging/logger.h"
#include "type/types.h"

namespace peloton {
namespace logging {

//===--------------------------------------------------------------------===//
// Backend Logger
//===--------------------------------------------------------------------===//

class BufferLogger : public Logger {
  friend class FlushLogger;

 public:
  BufferLogger();

  ~BufferLogger();

  static BufferLogger *GetBufferLogger(LoggingType logging_type);

  //===--------------------------------------------------------------------===//
  // Virtual Functions
  //===--------------------------------------------------------------------===//

  // Log the given record
  virtual void Log(LogRecord *record);

  std::vector<std::unique_ptr<LogBuffer>> &GetPreparedLogBuffers() { return prepared_buffers_for_flush; }

  std::pair<cid_t, cid_t> PrepareLogBuffers();

  // Grant an empty buffer to use
  void GrantEmptyBuffer(std::unique_ptr<LogBuffer>);

  // Set FrontendLoggerID
  void SetFrontendLoggerID(int id) { frontend_logger_id = id; }

  // Get FrontendLoggerID
  int GetFrontendLoggerID() { return frontend_logger_id; }

  // set when the frontend is dying
  void SetShutdown(bool);

  // gets the Varlenpool used for log serialization
  type::AbstractPool *GetVarlenPool() { return backend_pool.get(); }

 protected:
  // the lock for the buffer being used currently
  Spinlock log_buffer_lock;

  // temporary prepared_buffers_for_flush used by backend
  std::vector<std::unique_ptr<LogBuffer>> prepared_buffers_for_flush;

  // commit id of the highest value committed so far
  cid_t highest_logged_commit_message = INVALID_CID;

  // id of the corresponding frontend logger
  // int frontend_logger_id = -1;  // default

  // max cid for the current log buffer
  // cid_t max_log_id_buffer = 0;

  // the current buffer
  std::unique_ptr<LogBuffer> log_buffer_;

  // the pool of available buffers
  std::unique_ptr<BufferPool> available_buffer_pool_;

  // the pool of buffers to persist
  std::unique_ptr<BufferPool> persist_buffer_pool_;

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
}  // namespace peloton
