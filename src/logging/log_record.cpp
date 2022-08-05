//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// buffer_pool_manager_instance.cpp
//
// Identification: src/buffer/buffer_pool_manager.cpp
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "logging/log_record.h"
#include <sstream>

namespace fulgurDB {
namespace logging {

/**
 * @brief Serialize given data
 * @return true if we serialize data otherwise false
 */
bool LogRecord::Serialize() {
  // TODO
  return false;
}

/**
 * @brief Serialize LogRecordHeader
 */
void LogRecord::SerializeHeader() {
  // TODO:
}

/**
 * @brief Deserialize LogRecordHeader
 */
void LogRecord::DeserializeHeader() {
  // TODO
}

const std::string LogRecord::GetInfo() const {
  std::ostringstream os;

  os << "log_record_type: " << LogRecordTypeToString(GetType()) << "\n";
  os << "db_oid: " << GetDatabaseOid() << "\n";
  os << "table_oid: " << GetTableId() << "\n";
  os << "txn_id: " << GetTransactionId() << "\n";
  os << "tuple_location: " << GetTupleLocation().block;
  os << " " << GetTupleLocation().offset << "\n";
  os << "message_length: " << GetMessageLength() << "\n";
  os << "\n";

  return os.str();
}

}  // namespace logging
}  // namespace fulgurDB
