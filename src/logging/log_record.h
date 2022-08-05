#include <iostream>

#include "logging/otherone/item_pointer.h"
#include "logging/type.h"

namespace fulgurDB {
namespace logging {

//===--------------------------------------------------------------------===//
// LogRecord
//===--------------------------------------------------------------------===//

// [] 1. tuple 的序列化以及反序列化由不由我来写 ？ tuple_size就也不由我来控制了？
// [] 2. 序列化以及反序列化所需的Serialize.io辅助类可以搬运过来用么（按道理可以，但是逻辑就太相似了）
// [ok] 3. 有没有必要将header 的序列化单独拎出来？（有必要）
// tuple_data 实际上是tuple本身（不是序列化后的数据），其序列化是在LogRecord::serialize中实现（或者调用）的
class LogRecord {
 public:
  LogRecord(LogRecordType log_record_type, const cid_t txn_id, oid_t table_oid, ItemPointer tuple_location,
            const void *tuple_data = nullptr, oid_t _db_oid = INVALID_OID)
      : log_record_type(log_record_type),
        txn_id(txn_id),
        table_oid(table_oid),
        tuple_location(tuple_location),
        tuple_data(tuple_data),
        db_oid(_db_oid) {}

  ~LogRecord() { delete[] message; }

  bool Serialize();

  void SerializeHeader();

  void DeserializeHeader();

  // 将信息string化，用于debug
  const std::string GetInfo() const;

  //===--------------------------------------------------------------------===//
  // 无需在cpp中实现的工具类函数
  //===--------------------------------------------------------------------===//
  LogRecordType GetType() const { return log_record_type; }

  cid_t GetTransactionId() const { return txn_id; }

  oid_t GetTableId(void) const { return table_oid; }

  oid_t GetDatabaseOid() const { return db_oid; }

  ItemPointer GetTupleLocation(void) const { return tuple_location; }

  char *GetMessage(void) const { return message; }

  size_t GetMessageLength(void) const { return message_length; }

  // ItemPointer GetDeleteLocation(void) const { return delete_location; }
  // void SetTuple(storage::Tuple *tuple);
  // storage::Tuple *GetTuple();
  // static size_t GetTupleRecordSize(void);

 private:
  LogRecordType log_record_type = LOGRECORD_TYPE_INVALID;

  // transaction id
  txn_id_t txn_id;

  // table id
  oid_t table_oid = INVALID_OID;

  // tuple location
  ItemPointer tuple_location;

  // serialized tuple data
  const void *tuple_data;
  // size_t tuple_size;

  // database id
  oid_t db_oid = 0;

  // serialized recored message
  char *message = nullptr;
  size_t message_length = 0;

  // tuple (for deserialize
  // storage::Tuple *tuple = nullptr;
};

}  // namespace logging
}  // namespace fulgurDB