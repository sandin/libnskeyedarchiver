#include "nskeyedarchiver/nskeyedunarchiver.hpp"

#include <gtest/gtest.h>
#include <sys/stat.h>

#include <string>
#include <typeinfo>

#include "nskeyedarchiver/common.hpp"
#include "nskeyedarchiver/kaarray.hpp"
#include "nskeyedarchiver/kamap.hpp"
#include "nskeyedarchiver/kastring.hpp"
#include "nskeyedarchiver/kavalue.hpp"
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
    hexdump(buffer, buffer_size, 0);                                       \
  } while (0)

TEST(NSKeyedUnarchiverTest, DecodeObject_NSDictionary_SetConfig) {
  // TEST DATA:
  // ```
  // {
  //    "rp": 10,
  //    "ur": 500,
  //    "tc": [
  //        {
  //            "uuid": "2C46B61A-CDA9-4D59-B901-22E28B08C260",
  //            "tk": 3,
  //            "kdf2": [630784000, 833617920, 830472456]
  //        }
  //    ]
  // }
  // ```
  char* buffer = nullptr;
  size_t buffer_size = 0;
  READ_CONTENT_FROM_FILE("nsdictionary_setconfig.bplist");
  auto guard = make_scope_exit([&]() { free(buffer); });

  KAValue obj = NSKeyedUnarchiver::UnarchiveTopLevelObjectWithData(buffer, (uint32_t)buffer_size);
  ASSERT_FALSE(obj.IsNull());
  ASSERT_TRUE(obj.IsObject());

  const KAMap& root = obj.ToObject<KAMap>();
  printf("root typeid: %s.\n", typeid(root).name());
  ASSERT_STREQ(root.ClassName().c_str(), "NSDictionary");
  ASSERT_EQ(3, root.Size());

  const KAValue& ur = root.at("ur");
  ASSERT_TRUE(ur.IsInteger());
  ASSERT_EQ(500, ur.ToInteger());

  const KAValue& rp = root.at("rp");
  ASSERT_TRUE(rp.IsInteger());
  ASSERT_EQ(10, rp.ToInteger());

  const KAValue& tc = root.at("tc");
  ASSERT_TRUE(tc.IsObject());
  const KAArray& tc_arr = tc.ToObject<KAArray>();
  ASSERT_STREQ("NSArray", tc_arr.ClassName().c_str());
  ASSERT_EQ(1, tc_arr.Size());

  const KAValue& tc_item_0 = tc_arr.at(0);
  ASSERT_TRUE(tc_item_0.IsObject());
  const KAMap& tc_item_map = tc_item_0.ToObject<KAMap>();
  ASSERT_STREQ("NSMutableDictionary", tc_item_map.ClassName().c_str());
  ASSERT_EQ(3, tc_item_map.Size());

  const KAValue& uuid = tc_item_map.at("uuid");
  ASSERT_TRUE(uuid.IsObject());
  const KAString& uuid_string = uuid.ToObject<KAString>();
  ASSERT_STREQ("NSMutableString", uuid_string.ClassName().c_str());
  ASSERT_STREQ("2C46B61A-CDA9-4D59-B901-22E28B08C260", uuid_string.ToString().c_str());

  const KAValue& tk = tc_item_map.at("tk");
  ASSERT_TRUE(tk.IsInteger());
  ASSERT_EQ(3, tk.ToInteger());

  const KAValue& kdf2 = tc_item_map.at("kdf2");
  ASSERT_TRUE(kdf2.IsObject());
  const KAArray& kdf2_set = kdf2.ToObject<KAArray>();
  ASSERT_STREQ("NSMutableSet", kdf2_set.ClassName().c_str());
  ASSERT_EQ(3, kdf2_set.Size());

  const KAValue& kdf2_set_item_0 = kdf2_set.at(0);
  ASSERT_TRUE(kdf2_set_item_0.IsInteger());
  ASSERT_EQ(630784000, kdf2_set_item_0.ToInteger());
  ASSERT_EQ(KAValue(630784000), kdf2_set_item_0);

  const KAValue& kdf2_set_item_1 = kdf2_set.at(1);
  ASSERT_TRUE(kdf2_set_item_1.IsInteger());
  ASSERT_EQ(833617920, kdf2_set_item_1.ToInteger());

  const KAValue& kdf2_set_item_2 = kdf2_set.at(2);
  ASSERT_TRUE(kdf2_set_item_2.IsInteger());
  ASSERT_EQ(830472456, kdf2_set_item_2.ToInteger());
}

TEST(NSKeyedUnarchiverTest, DecodeObject_NSSet_Energy) {
  // TEST DATA:
  // ```
  // [
  //    "energy.cost", "energy.CPU", "energy.networking", "energy.location",
  //    "energy.GPU", "energy.appstate", "energy.overhead"
  // ]
  // ```
  char* buffer = nullptr;
  size_t buffer_size = 0;
  READ_CONTENT_FROM_FILE("nsset_energy.bplist");
  auto guard = make_scope_exit([&]() { free(buffer); });

  KAValue obj = NSKeyedUnarchiver::UnarchiveTopLevelObjectWithData(buffer, (uint32_t)buffer_size);
  ASSERT_FALSE(obj.IsNull());
  ASSERT_TRUE(obj.IsObject());

  const KAArray& root = obj.ToObject<KAArray>();
  ASSERT_STREQ(root.ClassName().c_str(), "NSSet");
  ASSERT_EQ(7, root.Size());

  ASSERT_EQ(KAValue("energy.cost"), root.at(0));
  ASSERT_EQ(KAValue("energy.CPU"), root.at(1));
  ASSERT_EQ(KAValue("energy.networking"), root.at(2));
  ASSERT_EQ(KAValue("energy.location"), root.at(3));
  ASSERT_EQ(KAValue("energy.GPU"), root.at(4));
  ASSERT_EQ(KAValue("energy.appstate"), root.at(5));
  ASSERT_EQ(KAValue("energy.overhead"), root.at(6));
}

TEST(NSKeyedUnarchiverTest, DecodeString) {
  // TEST DATA:
  // ```
  // "setConfig:"
  // ```
  char* buffer = nullptr;
  size_t buffer_size = 0;
  READ_CONTENT_FROM_FILE("nsstring_setconfig.bplist");
  auto guard = make_scope_exit([&]() { free(buffer); });

  KAValue obj = NSKeyedUnarchiver::UnarchiveTopLevelObjectWithData(buffer, (uint32_t)buffer_size);
  ASSERT_FALSE(obj.IsNull());
  ASSERT_TRUE(obj.IsStr());
  const char* val = obj.ToStr();
  printf("val=%s\n", val);
  EXPECT_STREQ(val, "setConfig:");
}

TEST(NSKeyedUnarchiverTest, DecodeUInt) {
  // TEST DATA:
  // ```
  // 10
  // ```
  char* buffer = nullptr;
  size_t buffer_size = 0;
  READ_CONTENT_FROM_FILE("integer_10.bplist");
  auto guard = make_scope_exit([&]() { free(buffer); });

  KAValue obj = NSKeyedUnarchiver::UnarchiveTopLevelObjectWithData(buffer, (uint32_t)buffer_size);
  ASSERT_FALSE(obj.IsNull());
  ASSERT_TRUE(obj.IsInteger());
  uint64_t val = obj.ToInteger();
  printf("val=%llu\n", val);
  EXPECT_EQ(val, 10);
}

TEST(NSKeyedUnarchiverTest, DecodeBool) {
  // TEST DATA:
  // ```
  // true
  // ```
  char* buffer = nullptr;
  size_t buffer_size = 0;
  READ_CONTENT_FROM_FILE("bool_true.bplist");
  auto guard = make_scope_exit([&]() { free(buffer); });

  KAValue obj = NSKeyedUnarchiver::UnarchiveTopLevelObjectWithData(buffer, (uint32_t)buffer_size);
  ASSERT_FALSE(obj.IsNull());
  ASSERT_TRUE(obj.IsBool());
  bool val = obj.ToBool();
  printf("val=%d\n", val);
  EXPECT_EQ(val, true);
}
