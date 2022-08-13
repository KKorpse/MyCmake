#pragma once

#include <cstdint>
#include <limits>
#include <string>

namespace fulgurDB {

//===--------------------------------------------------------------------===//
// Type definitions.
//===--------------------------------------------------------------------===//

typedef uint32_t oid_t;

static const oid_t INVALID_OID = std::numeric_limits<oid_t>::max();
// For transaction id

typedef uint64_t txn_id_t;

static const txn_id_t INVALID_TXN_ID = 0;

static const txn_id_t MAX_TXN_ID = std::numeric_limits<txn_id_t>::max();

// For commit id

typedef uint64_t cid_t;

static const cid_t INVALID_CID = 0;

static const cid_t MAX_CID = std::numeric_limits<cid_t>::max();

enum LogRecordType : unsigned char {
  LOGRECORD_TYPE_INVALID = '0',

  // 中间的普通记录
  LOGRECORD_TYPE_INSERT = 'i',
  LOGRECORD_TYPE_DELETE = 'd',
  LOGRECORD_TYPE_UPDATE = 'u',

  // 第一条记录标记为 BEGIN
  LOGRECORD_TYPE_INSERT_BEGIN = 'T',
  LOGRECORD_TYPE_DELETE_BEGIN = 'D',
  LOGRECORD_TYPE_UPDATE_BEGIN = 'U',

  // 最后一条记录标记为 COMMIT
  LOGRECORD_TYPE_INSERT_COMMIT = 'a',
  LOGRECORD_TYPE_DELETE_COMMIT = 'b',
  LOGRECORD_TYPE_UPDATE_COMMIT = 'c',

  // 暂时不知道啥用
  // Record for delimiting transactions
  // includes max persistent commit_id
  LOGRECORD_TYPE_ITERATION_DELIMITER = 'x',
};

std::string LogRecordTypeToString(LogRecordType type) {
  switch (type) {
    case LOGRECORD_TYPE_INVALID: {
      return "INVALID";
    }
    case LOGRECORD_TYPE_INSERT: {
      return "LOGRECORD_TYPE_INSERT";
    }
    case LOGRECORD_TYPE_DELETE: {
      return "LOGRECORD_TYPE_DELETE";
    }
    case LOGRECORD_TYPE_UPDATE: {
      return "LOGRECORD_TYPE_UPDATE";
    }
    case LOGRECORD_TYPE_INSERT_BEGIN: {
      return "LOGRECORD_TYPE_INSERT_BEGIN";
    }
    case LOGRECORD_TYPE_DELETE_BEGIN: {
      return "LOGRECORD_TYPE_DELETE_BEGIN";
    }
    case LOGRECORD_TYPE_UPDATE_BEGIN: {
      return "LOGRECORD_TYPE_UPDATE_BEGIN";
    }
    case LOGRECORD_TYPE_INSERT_COMMIT: {
      return "LOGRECORD_TYPE_INSERT_COMMIT";
    }
    case LOGRECORD_TYPE_DELETE_COMMIT: {
      return "LOGRECORD_TYPE_DELETE_COMMIT";
    }
    case LOGRECORD_TYPE_ITERATION_DELIMITER: {
      return "ITERATION_DELIMITER";
    }
    default: {
      return "INVALID";
    }
  }
  return "INVALID";
}

//===--------------------------------------------------------------------===//
// File Handle
//===--------------------------------------------------------------------===//
struct FileHandle {
  // FILE pointer
  FILE *file = nullptr;

  // File descriptor
  int fd;

  // Size of the file
  size_t size;

  FileHandle() : file(nullptr), fd(0), size(0) {}

  FileHandle(FILE *file, int fd, size_t size)
      : file(file), fd(fd), size(size) {}
};

// Data stored in AAA
// Log stored in BBB
enum class LoggingType {
  INVALID = 0,

  NVM_WAL = 1,
  SSD_WAL = 2,
  HDD_WAL = 3,
};

}  // namespace fulgurDB
