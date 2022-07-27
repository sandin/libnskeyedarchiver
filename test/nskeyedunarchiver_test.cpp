#include "nskeyedarchiver/nskeyedunarchiver.hpp"

#include <gtest/gtest.h>
#include <sys/stat.h>

#include <string>
#include <typeinfo>

#include "nskeyedarchiver/kaarray.hpp"
#include "nskeyedarchiver/kamap.hpp"
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

  KAMap* root = static_cast<KAMap*>(obj.ToObject());
  printf("root typeid: %s.\n", typeid(root).name());
  ASSERT_STREQ(root->ClassName().c_str(), "NSDictionary");
  ASSERT_EQ(3, root->Size());

  /*
  const KAMap& root = obj.ToObject<KAMap>();
  printf("root ptr: %p.\n", &root);
  printf("root typeid: %s.\n", typeid(root).name());
  printf("UnarchiveTopLevelObjectWithData, root.className=%s\n", root.ClassName().c_str());
  ASSERT_STREQ(root.ClassName().c_str(), "NSDictionary");
  ASSERT_EQ(3, root.Size());
  */

  KAValue& ur = (*root)["ur"];
  ASSERT_TRUE(ur.IsInteger());
  ASSERT_EQ(500, ur.ToInteger());

  KAValue& rp = (*root)["rp"];
  ASSERT_TRUE(rp.IsInteger());
  ASSERT_EQ(10, rp.ToInteger());

  KAValue& tc = (*root)["tc"];
  ASSERT_TRUE(tc.IsObject());
  KAArray* tc_arr = static_cast<KAArray*>(tc.ToObject());
  ASSERT_EQ(1, tc_arr->Size());

  // TODO:
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
