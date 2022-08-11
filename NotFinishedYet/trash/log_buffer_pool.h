/*

用于 BakendLogger 装载序列化的日志文件，类似于circle_pool

*/

#include "logging/type.h"

// 默认容量为 16
#define BUFFER_POOL_SIZE 16

namespace furgurDB {
namespace logging {

//===--------------------------------------------------------------------===//
// Log Buffer Pool
//===--------------------------------------------------------------------===//
class LogBufferPool {
 public:
  LogBufferPool();

  ~LogBufferPool();

  // put a buffer to buffer pool. blocks if over capacity
  bool Put(std::unique_ptr<LogBuffer>);

  // get all the buffer from buffer pool. blocks if none available
  std::unique_ptr<LogBuffer> GetAllBuffers(std::vector<std::unique_ptr<LogBuffer>> &buffer_queue);

  // get the number of buffers available
  unsigned int GetSize() { return size_.load(); }

 private:
  std::unique_ptr<LogBuffer> buffers_[BUFFER_POOL_SIZE];
  
  // 没必要线程安全吧。
  // unsigned int size_;

  // 有必要线程安全，这个size_可以用作插入的互斥资源数量
  std::atomic<unsigned int> size_;

  unsigned int capacity_; //  = BUFFER_POOL_SIZE;
  txn_id_t max_commited_txn_id; // = INVALID_TXN_ID;

  // 用来使此数据结构线程安全
  std::mutex latch_;
};

}  // namespace logging
}  // namespace furgurDB
