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

#include "logging/records/log_record.h"

int LogRecord::PrintSomething(int a) {
  std::cout << "a: " << a << std::endl;
  return a;
}
