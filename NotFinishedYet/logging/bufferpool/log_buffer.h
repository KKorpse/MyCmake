
#pragma once

#include <cstddef>
#include <memory>

#include "logging/log_record.h"
#include "common/macros.h"

namespace fulgurDB {
namespace logging {

// class BufferLogger;

//===--------------------------------------------------------------------===//
// Log Buffer
//===--------------------------------------------------------------------===//
class LogBuffer {
 public:
  // LogBuffer(BufferLogger *);
  LogBuffer();

  ~LogBuffer(void){};

  // get serialized data field
  char *GetData() { return elastic_data_.get(); }

  // serialize and write a log record to buffer
  bool WriteRecord(LogRecord *);

  // clean up and reset content
  void ResetData();

  inline size_t GetSize() { return size_; }

  inline void SetSize(size_t size) {
    PL_ASSERT(size < capacity_);
    size_ = size;
  }

  inline void SetMaxLogId(cid_t new_max) { max_log_id = new_max; }

  inline cid_t GetMaxLogId() { return max_log_id; }

  // inline BufferLogger *GetBufferLogger() { return buffer_logger_; }

 private:
  // write data to the log buffer, return false if not enough space
  bool WriteData(char *data, size_t len);

  // the size of buffer used already
  size_t size_ = 0;

  // the total capacity of the buffer
  size_t capacity_;

  // Dynamically adjusted data array
  std::unique_ptr<char[]> elastic_data_;

  // BufferLogger *buffer_logger_;

  // maximum log id seen so far
  cid_t max_log_id = 0;
};

}  // namespace logging
}  // namespace fulgurDB
