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

#include "logging/records/log_record.h"
#include "gtest/gtest.h"

TEST(MyTestTest, SampleTest) {
  LogRecord f;
  int r = f.PrintSomething(10);
  EXPECT_EQ(r, 10);
}
