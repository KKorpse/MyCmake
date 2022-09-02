
#pragma once

#include <condition_variable>
#include <mutex>
#include <vector>

#include "logging/otherone/item_pointer.h"
// #include "common/platform.h"
#include "logging/bufferpool/log_buffer.h"
#include "logging/log_record.h"
// #include "logging/loggers/logger.h"
#include "logging/types.h"

// for Debug
#include "logging/logger.h"

namespace fulgurDB {
namespace logging {

//===--------------------------------------------------------------------===//
// CheckPoint Logger
//===--------------------------------------------------------------------===//

class CheckPointer{

 public:

  // 循环检测 LogManager 发出的讯息？
  // 检查点需要是独立的线程
  // void MainLoop();

  // 执行检查点
  void DoCheckPoint();

  // 恢复检查点时，不用使用主循环，调用函数生成线程就行。
  // 从最近的检查点开始恢复，可以考虑在这里生成多线程进行恢复。
  bool DoRecovery();

  // 用于寻找检查点文件是否存在
  bool HasCheckPoint();


  // 重建索引（恢复完成之后建立）
  void RecoverIndex();
  
  // 恢复时只需要执行插入
  void InsertTuple(TupleRecord *recovery_txn);
 private:

  // 初始化检查点目录
  void InitDirectory()

  // 
};

}  // namespace logging
}  // namespace fulgurDB
