//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// buffer_pool_manager_instance_test.cpp
//
// Identification: test/buffer/buffer_pool_manager_test.cpp
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

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

  LogRecord L = LogRecord(log_record_type, txn_id, table_oid, tuple_location);
  std::cout << L.GetInfo();
}

} // namespace logging
} // namespace fulgurDB