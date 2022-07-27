#include "nskeyedarchiver/nskeyedunarchiver.hpp"

#include <gtest/gtest.h>
#include <sys/stat.h>

#include "nskeyedarchiver/scope.hpp"

using namespace nskeyedarchiver;

#define TEST_DIR "../../test/data/"
#define READ_CONTENT_FROM_FILE(filename)                                   \
  do {                                                                     \
    FILE* f = fopen(TEST_DIR filename, "rb");                              \
    if (!f) {                                                              \
      printf("can not open `%s` file\n", TEST_DIR filename);               \
      EXPECT_TRUE(false);                                                  \
      return;                                                              \
    }                                                                      \
    struct stat filestats;                                                 \
    stat(TEST_DIR filename, &filestats);                                   \
    buffer_size = filestats.st_size;                                       \
    if (buffer_size == 0) {                                                \
      printf("can not get `%s` file size\n", TEST_DIR filename);           \
      EXPECT_TRUE(false);                                                  \
      return;                                                              \
    }                                                                      \
    buffer = (char*)malloc(buffer_size);                                   \
    fread(buffer, 1, buffer_size, f);                                      \
    fclose(f);                                                             \
                                                                           \
    if (buffer == nullptr) {                                               \
      printf("can not read `%s` file\n", TEST_DIR filename);               \
      EXPECT_TRUE(false);                                                  \
      return;                                                              \
    }                                                                      \
    printf("filename=%s, buffer=%p, buffer_size=%zu\n", TEST_DIR filename, \
           static_cast<void*>(buffer), buffer_size);                       \
    auto guard = make_scope_exit([&]() { free(buffer); });                 \
  } while (0)

TEST(NSKeyedUnarchiverTest, DecodeObject) {
  char* buffer = nullptr;
  size_t buffer_size = 0;
  READ_CONTENT_FROM_FILE("setconfig.bplist");

  KAValue obj = NSKeyedUnarchiver::UnarchiveTopLevelObjectWithData(buffer, (uint32_t)buffer_size);
  ASSERT_FALSE(obj.IsNull());
  ASSERT_TRUE(obj.IsObject());
  const KAObject& root = obj.ToObject();
  printf("UnarchiveTopLevelObjectWithData, root.className=%s\n", root.ClassName().c_str());
}

TEST(NSKeyedUnarchiverTest, DecodeString) {
  char* buffer = nullptr;
  size_t buffer_size = 0;
  READ_CONTENT_FROM_FILE("str_setconfig.bplist");

  KAValue obj = NSKeyedUnarchiver::UnarchiveTopLevelObjectWithData(buffer, (uint32_t)buffer_size);
  ASSERT_FALSE(obj.IsNull());
  ASSERT_TRUE(obj.IsStr());
  const char* val = obj.ToStr();
  printf("val=%s\n", val);
  EXPECT_STREQ(val, "setConfig:");
}

TEST(NSKeyedUnarchiverTest, DecodeUInt) {
  char* buffer = nullptr;
  size_t buffer_size = 0;
  READ_CONTENT_FROM_FILE("uint_10.bplist");

  KAValue obj = NSKeyedUnarchiver::UnarchiveTopLevelObjectWithData(buffer, (uint32_t)buffer_size);
  ASSERT_FALSE(obj.IsNull());
  ASSERT_TRUE(obj.IsInteger());
  uint64_t val = obj.ToInteger();
  printf("val=%llu\n", val);
  EXPECT_EQ(val, 10);
}

TEST(NSKeyedUnarchiverTest, DecodeBool) {
  char* buffer = nullptr;
  size_t buffer_size = 0;
  READ_CONTENT_FROM_FILE("bool_true.bplist");

  KAValue obj = NSKeyedUnarchiver::UnarchiveTopLevelObjectWithData(buffer, (uint32_t)buffer_size);
  ASSERT_FALSE(obj.IsNull());
  ASSERT_TRUE(obj.IsBool());
  bool val = obj.ToBool();
  printf("val=%d\n", val);
  EXPECT_EQ(val, true);
}
