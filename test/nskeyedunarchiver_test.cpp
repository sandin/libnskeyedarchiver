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
  char* buffer = nullptr;
  size_t buffer_size = 0;
  READ_CONTENT_FROM_FILE("nsdictionary_setconfig.bplist");
  auto guard = make_scope_exit([&]() { free(buffer); });

  KAValue obj = NSKeyedUnarchiver::UnarchiveTopLevelObjectWithData(buffer, (uint32_t)buffer_size);
  ASSERT_FALSE(obj.IsNull());
  ASSERT_TRUE(obj.IsObject());

  const KAMap& root = obj.AsObject<KAMap>();
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
  const KAArray& tc_arr = tc.AsObject<KAArray>();
  ASSERT_STREQ("NSArray", tc_arr.ClassName().c_str());
  ASSERT_EQ(1, tc_arr.Size());

  const KAValue& tc_item_0 = tc_arr.at(0);
  ASSERT_TRUE(tc_item_0.IsObject());
  const KAMap& tc_item_map = tc_item_0.AsObject<KAMap>();
  ASSERT_STREQ("NSMutableDictionary", tc_item_map.ClassName().c_str());
  ASSERT_EQ(3, tc_item_map.Size());

  const KAValue& uuid = tc_item_map.at("uuid");
  ASSERT_TRUE(uuid.IsObject());
  const KAString& uuid_string = uuid.AsObject<KAString>();
  ASSERT_STREQ("NSMutableString", uuid_string.ClassName().c_str());
  ASSERT_STREQ("2C46B61A-CDA9-4D59-B901-22E28B08C260", uuid_string.ToString().c_str());

  const KAValue& tk = tc_item_map.at("tk");
  ASSERT_TRUE(tk.IsInteger());
  ASSERT_EQ(3, tk.ToInteger());

  const KAValue& kdf2 = tc_item_map.at("kdf2");
  ASSERT_TRUE(kdf2.IsObject());
  const KAArray& kdf2_set = kdf2.AsObject<KAArray>();
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

  std::string json = root.ToJson();
  printf("%s\n", json.c_str());
  ASSERT_STREQ(
      R"({rp:10,tc:[{kdf2:[630784000,833617920,830472456],tk:3,uuid:"2C46B61A-CDA9-4D59-B901-22E28B08C260"}],ur:500})",
      json.c_str());
}

TEST(NSKeyedUnarchiverTest, DecodeObject_NSArray_GpuInfo) {
  char* buffer = nullptr;
  size_t buffer_size = 0;
  READ_CONTENT_FROM_FILE("nsarray_gpuinfo.bplist");
  auto guard = make_scope_exit([&]() { free(buffer); });

  KAValue obj = NSKeyedUnarchiver::UnarchiveTopLevelObjectWithData(buffer, (uint32_t)buffer_size);
  ASSERT_FALSE(obj.IsNull());
  ASSERT_TRUE(obj.IsObject());

  const KAArray& root = obj.AsObject<KAArray>();
  ASSERT_STREQ(root.ClassName().c_str(), "NSMutableArray");
  ASSERT_EQ(1, root.Size());

  const KAMap& item_0 = root.at(0).AsObject<KAMap>();
  ASSERT_STREQ(item_0.ClassName().c_str(), "NSDictionary");
  ASSERT_EQ(15, item_0.Size());

  ASSERT_EQ(KAValue(4294967998LL), item_0.at("accelerator-id"));
  ASSERT_EQ(KAValue("3.24.4"), item_0.at("agx-tracecode-version"));
  ASSERT_EQ(KAValue("A13"), item_0.at("device-name"));
  ASSERT_EQ("NSMutableArray", item_0.at("displays").AsObject<KAArray>().ClassName());
  ASSERT_EQ(KAValue("A13"), item_0.at("family-name"));
  ASSERT_EQ(KAValue(false), item_0.at("headless"));
  ASSERT_EQ(KAValue(false), item_0.at("low-power"));
  ASSERT_EQ(KAValue(0), item_0.at("min-collection-interval"));
  ASSERT_EQ(KAValue(true), item_0.at("mobile"));
  ASSERT_EQ("NSMutableDictionary", item_0.at("perf-state").AsObject<KAMap>().ClassName());
  ASSERT_EQ(KAValue(""), item_0.at("product-name"));
  ASSERT_EQ(
      KAValue(2620473344LL),
      item_0.at(
          "recommended-max-working-set-size"));  // https://developer.apple.com/documentation/metal/mtldevice/2369280-recommendedmaxworkingsetsize
  ASSERT_EQ(KAValue(false), item_0.at("removable"));
  ASSERT_EQ("NSMutableArray",
            item_0.at("supported-counter-profiles").AsObject<KAArray>().ClassName());
  ASSERT_EQ(KAValue("Apple"), item_0.at("vendor-name"));

  std::string json = root.ToJson();
  printf("%s\n", json.c_str());
  ASSERT_STREQ(
      R"([{"accelerator-id":4294967998,"agx-tracecode-version":"3.24.4","device-name":"A13","displays":[{"accelerator-id":4294967998,"built-in":true,"device-name":"LCD","display-id":1,"framebuffer-index":0,"pixel-height":2688.000000,"pixel-width":1242.000000,"resolution":[1242.000000,2688.000000]}],"family-name":"A13","headless":false,"low-power":false,"min-collection-interval":0,"mobile":true,"perf-state":{"accelerator-id":4294967998,"available":true,"enabled":false,"level":0,"mapping":16975360,"sustained":false},"product-name":"","recommended-max-working-set-size":2620473344,"removable":false,"supported-counter-profiles":[0,3,4,1],"vendor-name":"Apple"}])",
      json.c_str());
}

TEST(NSKeyedUnarchiverTest, DecodeObject_NSArray_GpuCounters) {
  char* buffer = nullptr;
  size_t buffer_size = 0;
  READ_CONTENT_FROM_FILE("nsarray_gpucounters.bplist");
  auto guard = make_scope_exit([&]() { free(buffer); });

  KAValue obj = NSKeyedUnarchiver::UnarchiveTopLevelObjectWithData(buffer, (uint32_t)buffer_size);
  ASSERT_FALSE(obj.IsNull());
  ASSERT_TRUE(obj.IsObject());

  const KAArray& root = obj.AsObject<KAArray>();
  printf("root typeid: %s.\n", typeid(root).name());
  ASSERT_STREQ(root.ClassName().c_str(), "NSArray");
  ASSERT_EQ(6, root.Size());

  std::string json = root.ToJson();
  printf("%s\n", json.c_str());
  // TODO:
  // ASSERT_STREQ(R"({rp:10,tc:[{kdf2:[630784000,833617920,830472456],tk:3,uuid:"2C46B61A-CDA9-4D59-B901-22E28B08C260"}],ur:500})",
  // json.c_str());
}

TEST(NSKeyedUnarchiverTest, DecodeObject_NSArray_RunningProcesses) {
  char* buffer = nullptr;
  size_t buffer_size = 0;
  READ_CONTENT_FROM_FILE("nsarray_runningprocesses.bplist");
  auto guard = make_scope_exit([&]() { free(buffer); });

  KAValue obj = NSKeyedUnarchiver::UnarchiveTopLevelObjectWithData(buffer, (uint32_t)buffer_size);
  ASSERT_FALSE(obj.IsNull());
  ASSERT_TRUE(obj.IsObject());

  const KAArray& root = obj.AsObject<KAArray>();
  ASSERT_STREQ(root.ClassName().c_str(), "NSMutableArray");
  ASSERT_EQ(328, root.Size());

  // the first one
  ASSERT_EQ(KAValue(84474), root.at(0).AsObject<KAMap>().at("pid"));
  ASSERT_EQ(KAValue("contactsd"), root.at(0).AsObject<KAMap>().at("name"));
  ASSERT_EQ(KAValue("/System/Library/Frameworks/Contacts.framework/Support/contactsd"),
            root.at(root.Size() - 1).AsObject<KAMap>().at("realAppName"));
  ASSERT_EQ(KAValue(false), root.at(0).AsObject<KAMap>().at("isApplication"));

  // the last one
  ASSERT_EQ(KAValue(0), root.at(root.Size() - 1).AsObject<KAMap>().at("pid"));
  ASSERT_EQ(KAValue("Mach Kernel"), root.at(root.Size() - 1).AsObject<KAMap>().at("name"));
  ASSERT_EQ(KAValue("mach_kernel"), root.at(root.Size() - 1).AsObject<KAMap>().at("realAppName"));
  ASSERT_EQ(KAValue(false), root.at(root.Size() - 1).AsObject<KAMap>().at("isApplication"));

  std::string json = root.ToJson();
  printf("%s\n", json.c_str());
}

TEST(NSKeyedUnarchiverTest, DecodeObject_NSArray_Networking) {
  char* buffer = nullptr;
  size_t buffer_size = 0;
  READ_CONTENT_FROM_FILE("nsarray_networking.bplist");
  auto guard = make_scope_exit([&]() { free(buffer); });

  KAValue obj = NSKeyedUnarchiver::UnarchiveTopLevelObjectWithData(buffer, (uint32_t)buffer_size);
  ASSERT_FALSE(obj.IsNull());
  ASSERT_TRUE(obj.IsObject());

  const KAArray& root = obj.AsObject<KAArray>();
  ASSERT_STREQ(root.ClassName().c_str(), "NSArray");
  ASSERT_EQ(2, root.Size());

  // item 0: type 1(connection-detected)
  ASSERT_EQ(KAValue(1), root.at(0));

  // item 1: data
  const KAArray& data = root.at(1).AsObject<KAArray>();
  ASSERT_STREQ(data.ClassName().c_str(), "NSArray");
  ASSERT_EQ(8, data.Size());
  int i = 0;
  const KAValue& item0 = data.at(i++);  // LocalAddress, type = data
  ASSERT_TRUE(item0.IsRaw());
  ASSERT_STREQ(R"("HB7GdQAAAAAkCIVWIgBIRxQL4rBTNf1ZAAAAAA==")", item0.ToJson().c_str());

  const KAValue& item1 = data.at(i++);  // RemoeteAddress, type = data
  ASSERT_TRUE(item1.IsRaw());
  ASSERT_STREQ(R"("HB4AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA==")", item1.ToJson().c_str());

  ASSERT_EQ(KAValue(10), data.at(i++));      // InterfaceIndex
  ASSERT_EQ(KAValue(-2), data.at(i++));      // Pid
  ASSERT_EQ(KAValue(131072), data.at(i++));  // RecvBufferSize
  ASSERT_EQ(KAValue(0), data.at(i++));       // RecvBufferUsed
  ASSERT_EQ(KAValue(4), data.at(i++));       // SerialNumber
  ASSERT_EQ(KAValue(1), data.at(i++));       // Kind

  std::string json = root.ToJson();
  printf("%s\n", json.c_str());
  ASSERT_STREQ(
      R"([1,["HB7GdQAAAAAkCIVWIgBIRxQL4rBTNf1ZAAAAAA==","HB4AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA==",10,18446744073709551614,131072,0,4,1]])",
      json.c_str());
}

TEST(NSKeyedUnarchiverTest, DecodeObject_NSSet_Energy) {
  char* buffer = nullptr;
  size_t buffer_size = 0;
  READ_CONTENT_FROM_FILE("nsset_energy.bplist");
  auto guard = make_scope_exit([&]() { free(buffer); });

  KAValue obj = NSKeyedUnarchiver::UnarchiveTopLevelObjectWithData(buffer, (uint32_t)buffer_size);
  ASSERT_FALSE(obj.IsNull());
  ASSERT_TRUE(obj.IsObject());

  const KAArray& root = obj.AsObject<KAArray>();
  ASSERT_STREQ(root.ClassName().c_str(), "NSSet");
  ASSERT_EQ(7, root.Size());

  ASSERT_EQ(KAValue("energy.cost"), root.at(0));
  ASSERT_EQ(KAValue("energy.CPU"), root.at(1));
  ASSERT_EQ(KAValue("energy.networking"), root.at(2));
  ASSERT_EQ(KAValue("energy.location"), root.at(3));
  ASSERT_EQ(KAValue("energy.GPU"), root.at(4));
  ASSERT_EQ(KAValue("energy.appstate"), root.at(5));
  ASSERT_EQ(KAValue("energy.overhead"), root.at(6));

  std::string json = root.ToJson();
  printf("%s\n", json.c_str());
  ASSERT_STREQ(
      R"(["energy.cost","energy.CPU","energy.networking","energy.location","energy.GPU","energy.appstate","energy.overhead"])",
      json.c_str());
}

TEST(NSKeyedUnarchiverTest, DecodeString) {
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

  std::string json = obj.ToJson();
  printf("%s\n", json.c_str());
  ASSERT_STREQ(R"("setConfig:")", json.c_str());
}

TEST(NSKeyedUnarchiverTest, DecodeUInt) {
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

  std::string json = obj.ToJson();
  printf("%s\n", json.c_str());
  ASSERT_STREQ(R"(10)", json.c_str());
}

TEST(NSKeyedUnarchiverTest, DecodeBool) {
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

  std::string json = obj.ToJson();
  printf("%s\n", json.c_str());
  ASSERT_STREQ(R"(true)", json.c_str());
}
