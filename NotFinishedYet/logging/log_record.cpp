

#include "logging/log_record.h"
#include <sstream>

namespace fulgurDB {
namespace logging {

// TODO
bool LogRecord::Serialize() {
  /*

  */
  return false;
}

// TODO:
void LogRecord::SerializeHeader() {
}

// TODO
void LogRecord::DeserializeHeader() {

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
