

#include <atomic>
#include <string.h>
#include <xmmintrin.h>


#include "logging/loggers/log_buffer_pool.h"
#include "logging/logger.h"

namespace fulgurDB {
namespace logging {


/*

---BufferLogger线程中的逻辑---
buffer.lock()
while(true) {
  if (LogBufferPool.HasPlace()) {
    LogBufferPool->Put(buffer)
    buffer.unlock()
    break
  } else {
    buffer.unlock()
    _mm_pause()
  }
}


---FlushLogger线程中的逻辑---
buffer.lock()
LogBufferPool.GetAllBuffers()
buffer.unlock()


*/



//===--------------------------------------------------------------------===//
// Circular Buffer Pool
//===--------------------------------------------------------------------===//

LogBufferPool::LogBufferPool()
    : size_(ATOMIC_VAR_INIT(0)), capacity_(BUFFER_POOL_SIZE) {
  memset(buffers_, 0, BUFFER_POOL_SIZE * sizeof(std::unique_ptr<LogBuffer>));
}

LogBufferPool::~LogBufferPool() {
  for (unsigned int i = 0; i < size_; i++) {
    buffers_[i].release();
  }
}

bool LogBufferPool::Put(std::unique_ptr<LogBuffer> buffer) {
  // 池子满了就报错。
  while (true) {
    if (size_.load() < capacity_) {
      break;
    } else {
      // pause for a minimum amount of time
      _mm_pause();
    }
  }

  latch_.lock();
  unsigned int current_idx = size_.fetch_add(1);
  buffers_[current_idx] = std::move(buffer);
  latch_.unlock();

  return true;
}

std::unique_ptr<LogBuffer> GetAllBuffers(std::vector<std::unique_ptr<LogBuffer>> &buffer_queue) {
  latch_.lock();
  for (unsigned int i = 0; i < size_.load(); i++) {
    buffer_queue.push_back(std::move(buffers_[i]));
  }
  size_.exchange(0);
  latch_.unlock();

}

}  // namespace logging
}  // namespace fulgurDB
