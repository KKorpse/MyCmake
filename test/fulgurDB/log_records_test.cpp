

#include "gtest/gtest.h"
#include "logging/logger.h"

#include "logging/log_record.h"

#include <iostream>

namespace fulgurDB {
namespace logging {

TEST(MyTestTest, SampleTest) {
  LOG_INFO("fuck you \n");
  LogRecordType log_record_type = LOGRECORD_TYPE_INVALID;
  cid_t txn_id = 1;
  oid_t table_oid = 2;
  ItemPointer tuple_location = ItemPointer(3, 3);

  LogRecord L = LogRecord(log_record_type, txn_id, table_oid, tuple_location, tuple_location);
  std::cout << L.GetInfo();

  std::cout << UINT64_C(128) << std::endl;
}

}  // namespace logging
}  // namespace fulgurDB