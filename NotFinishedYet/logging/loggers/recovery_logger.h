#pragma once

#include "logging/loggers/logger.h"
#include "logging/log_record.h"
#include "logging/log_file.h"
// #include "executor/executors.h"

#include <dirent.h>
#include <vector>
#include <set>
#include <chrono>


namespace fulgurDB {
namespace logging {

//===--------------------------------------------------------------------===//
// Log Record Group
//===--------------------------------------------------------------------===//

// 用于装载同一个日志文件中读取到的，同一个事务ID对应的所有日志记录
// 因为同一个事务可能会分布在多个文件中，从而被多个 RecoveryLogger 发现，这类事务记录需要收集起来做合并
struct LogRecordGroup {
  std::vector<TupleRecord *> records;
  // version number 在文件名字中获取到，由 Flush Logger 设置，
  unsigned int version_number;
};

class Recovery_Logger : public Logger {
 public:
  Recovery_Logger(void);
  Recovery_Logger(unsigned int logger_id);

  ~Recovery_Logger(void);

  // 主要函数，调用其来实现恢复
  void DoRecovery(void);

  //===--------------------------------------------------------------------===//
  // 记录的处理
  //===--------------------------------------------------------------------===//

  // 索引需不需要在恢复数据时一条一条顺便恢复
  // void RecoverIndex();
  // 用于恢复索引，在执行某条事务的恢复时
  void InsertIndexEntry();

  // 获取到某个合法事务的第一个记录时，进行相关的操作
  // 在三种类型的table 找到合适的 table，创建对应的 key-word
  void StartTransactionRecovery(cid_t commit_id);

  // 某个事务的记录完整的获取到时，执行其恢复
  // 恢复后在table中删除此事务
  void CommitTransactionRecovery(cid_t commit_id);

  // 三种记录的恢复
  void InsertTuple(LogRecord *record);
  void DeleteTuple(LogRecord *record);
  void UpdateTuple(LogRecord *record);


  //===--------------------------------------------------------------------===//
  // 文件相关
  //===--------------------------------------------------------------------===//

  static constexpr auto log_directory_path = "log";

  // 根据本 Logger 的 id 来打开对应的文件，跟 RecoveryLogger 和 FlushLogger 的数量有关。
  void InitLogFilesList();

  // 一次性读取整个日志文件进入内存
  void ReadNextWholeLogFile();

  // 获取到的 version_number 用于新建 TupleRecordGroup
  std::string ExtractVersionNumberFromFileName(const char *name);

  // 按道理只有 CheckPoint 有权限删除日志文件
  // void DeleteLogFile();


  //===--------------------------------------------------------------------===//
  // 文件处理，记录获取
  //===--------------------------------------------------------------------===//

  // 从打开的当前文件中，【从尾向前】获取下一个记录的 Type
  LogRecordType GetNextLogRecordType();

  // 跳到下一条记录
  // 记录 cid 不满足条件时需要跳过
  // 当前记录读取之后也需要跳过
  bool SkipToNextRecord();
  
  // 获取记录的头部和身体，LogRecord 需要分别获取头部和 Tuple 本身。
  // 获取后，指针并不会指向下一个记录，也需要调用 SkipToNextRecord()
  bool ReadNextTupleRecordHeader(TupleRecord &tuple_record);
  storage::Tuple *ReadNextTupleRecordBody(const catalog::Schema *schema, type::AbstractPool *pool, FileHandle &file_handle);


 private:
  //===--------------------------------------------------------------------===//
  // 对应 【记录的处理】
  //===--------------------------------------------------------------------===//

  // 恢复时是否需要恢复索引
  bool should_recovery_idx = true;

  // 如果 RecoveryLogger 数量等于 FlushLogger 的数量，则设置为 false
  // 因为在这种情况下，合法的事务一定是完整的，只会使用到 txn_table_records_with_commit
  bool need_to_merge = true;

  // 分别装入三种类型的日志记录组
  std::map<txn_id_t, LogRecordGroup> txn_table_records_with_nothing;
  std::map<txn_id_t, LogRecordGroup> txn_table_records_with_begin;
  std::map<txn_id_t, LogRecordGroup> txn_table_records_with_commit;


  //===--------------------------------------------------------------------===//
  // 对应 【文件相关】
  //===--------------------------------------------------------------------===//

  // 存储所有跟此 Logger 相关的文件信息（句柄，名字等）
  std::vector<LogFile *> log_files_;


  //===--------------------------------------------------------------------===//
  // 对应 【文件处理，记录获取】
  //===--------------------------------------------------------------------===//

  // 当前读入内存的整个日志文件
  char *cur_file; // 指向此内存中文件的头部
  unsigned int file_len; // 此文件的长度 = cur_fule_cursor - cur_file
  char *cur_file_cursor; // 用于读取文件中的 LogRecord，从尾部开始



  // File pointer and descriptor
  // FileHandle cur_file_handle;

  // Keep tracking max oid for setting next_oid in manager
  // For active processing after recovery
  // oid_t max_oid = 0;
  // cid_t max_cid = 0;

  // pool for allocating non-inlined values
  // type::AbstractPool *recovery_pool;

  // 日志文件列表

};

}  // namespace logging
}  // namespace fulgurDB
