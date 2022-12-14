
#pragma once

#include <atomic>
#include <cstring>

#include "logging/buffer_pool.h"
#include "common/logger.h"

#define BUFFER_POOL_MASK (BUFFER_POOL_SIZE - 1)
#define GET_BUFFER_POOL_INDEX(a) (a & BUFFER_POOL_MASK)

namespace fulgurDB {
namespace logging {

//===--------------------------------------------------------------------===//
// Circular Buffer Pool
//===--------------------------------------------------------------------===//
class CircularBufferPool : public BufferPool {
 public:
  CircularBufferPool();

  ~CircularBufferPool();

  // put a buffer to buffer pool. blocks if over capacity
  bool Put(std::unique_ptr<LogBuffer>);

  // get a buffer from buffer pool. blocks if none available
  std::unique_ptr<LogBuffer> Get();

  // get the number of buffers available
  unsigned int GetSize();

 private:
  std::unique_ptr<LogBuffer> buffers_[BUFFER_POOL_SIZE];
  std::atomic<unsigned int> head_;
  std::atomic<unsigned int> tail_;
};

}  // namespace logging
}  // namespace fulgurDB
