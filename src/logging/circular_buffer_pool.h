

#pragma once

#include <atomic>
#include <cstring>
#include <memory>

#define BUFFER_POOL_MASK (BUFFER_POOL_SIZE - 1)
#define GET_BUFFER_POOL_INDEX(a) (a & BUFFER_POOL_MASK)
#define BUFFER_POOL_SIZE 1024 * 8

class LogBuffer {
 public: 
  LogBuffer() {
    data = new char [1024 * 128];
  }
  ~LogBuffer() {
    delete [] data;
  }
 private:
  char *data;//[1024 * 128];
};
//===--------------------------------------------------------------------===//
// Circular Buffer Pool
//===--------------------------------------------------------------------===//
class CircularBufferPool{
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
