#include "nskeyedarchiver/nskeyedunarchiver.hpp"

#include <gtest/gtest.h>
#include <sys/stat.h>

using namespace nskeyedarchiver;

void read_content_from_file(const char* filename, char** buffer,
                            size_t* buffer_size) {
  FILE* f = fopen(filename, "rb");
  if (!f) {
    return;
  }
  struct stat filestats;
  stat(filename, &filestats);
  *buffer_size = filestats.st_size;
  *buffer = (char*)malloc(*buffer_size);
  fread(*buffer, 1, *buffer_size, f);
  fclose(f);
}

#define READ_CONTENT_FROM_FILE(f) do { \
  read_content_from_file(f, &buffer, &buffer_size); \
  if (buffer == nullptr) { \
    printf("can not read %s file\n", f); \
    EXPECT_TRUE(false); \
    return; \
  } \
  printf("buffer=%p, buffer_size=%zu\n", static_cast<void*>(buffer), buffer_size); \
} while (0) 

/*
TEST(NSKeyedUnarchiverTest, DecodeObject) {
  char* buffer = nullptr; \
  size_t buffer_size = 0; \
  READ_CONTENT_FROM_FILE("../test/data/setconfig.bplist");

  NSObject* obj = NSKeyedUnarchiver::UnarchiveTopLevelObjectWithData(
      buffer, (uint32_t)buffer_size);
  ASSERT_TRUE(obj != nullptr);
  printf("UnarchiveTopLevelObjectWithData, obj=%s", obj->ToString().c_str());

  if (obj) {
    delete obj;
  }
  free(buffer);
}
*/

TEST(NSKeyedUnarchiverTest, DecodeString) {
  char* buffer = nullptr; \
  size_t buffer_size = 0; \
  READ_CONTENT_FROM_FILE("../../test/data/str_setconfig.bplist");

  KAValue obj = NSKeyedUnarchiver::UnarchiveTopLevelObjectWithData(buffer, (uint32_t)buffer_size);
  ASSERT_FALSE(obj.IsNull());
  ASSERT_TRUE(obj.IsStr());
  const char* val = obj.ToStr();
  printf("val=%s\n", val);
  EXPECT_STREQ(val, "setConfig:");

  free(buffer);
}

TEST(NSKeyedUnarchiverTest, DecodeUInt) {
  char* buffer = nullptr; \
  size_t buffer_size = 0; \
  READ_CONTENT_FROM_FILE("../../test/data/uint_10.bplist");

  KAValue obj = NSKeyedUnarchiver::UnarchiveTopLevelObjectWithData(buffer, (uint32_t)buffer_size);
  ASSERT_FALSE(obj.IsNull());
  ASSERT_TRUE(obj.IsInteger());
  uint64_t val = obj.ToInteger();
  printf("val=%llu\n", val);
  EXPECT_EQ(val, 10);

  free(buffer);
}

TEST(NSKeyedUnarchiverTest, DecodeBool) {
  char* buffer = nullptr; \
  size_t buffer_size = 0; \
  READ_CONTENT_FROM_FILE("../../test/data/bool_true.bplist");

  KAValue obj = NSKeyedUnarchiver::UnarchiveTopLevelObjectWithData(buffer, (uint32_t)buffer_size);
  ASSERT_FALSE(obj.IsNull());
  ASSERT_TRUE(obj.IsBool());
  bool val = obj.ToBool();
  printf("val=%d\n", val);
  EXPECT_EQ(val, true);

  free(buffer);
}
