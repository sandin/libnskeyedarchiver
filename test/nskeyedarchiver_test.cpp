#include "nskeyedarchiver/nskeyedarchiver.hpp"

#include <gtest/gtest.h>
#include <sys/stat.h>

#include <string>
#include <unordered_map>

#include "nskeyedarchiver/common.hpp"
#include "nskeyedarchiver/kaarray.hpp"
#include "nskeyedarchiver/kamap.hpp"
#include "nskeyedarchiver/kavalue.hpp"
#include "nskeyedarchiver/scope.hpp"

using namespace nskeyedarchiver;

#define ARCHIVED_DATA(object)                                                           \
  char* buffer = nullptr;                                                               \
  size_t buffer_size = 0;                                                               \
  NSKeyedArchiver::ArchivedData(object, &buffer, &buffer_size,                          \
                                NSKeyedArchiver::OutputFormat::Binary);                 \
  ASSERT_TRUE(buffer != nullptr);                                                       \
  ASSERT_TRUE(buffer_size > 0);                                                         \
  auto buffer_guard = make_scope_exit([&]() { free(buffer); });                         \
  plist_t plist;                                                                        \
  plist_from_bin(buffer, buffer_size, &plist);                                          \
  auto plist_guard = make_scope_exit([&]() { plist_free(plist); });                     \
                                                                                        \
  plist_type node_type = plist_get_node_type(plist);                                    \
  ASSERT_EQ(PLIST_DICT, node_type);                                                     \
  print_plist_as_xml(plist);                                                            \
                                                                                        \
  ASSERT_EQ(4, plist_dict_get_size(plist));                                             \
  uint64_t length;                                                                      \
  ASSERT_STREQ("NSKeyedArchiver",                                                       \
               plist_get_string_ptr(plist_dict_get_item(plist, "$archiver"), &length)); \
                                                                                        \
  uint64_t u;                                                                           \
  plist_get_uint_val(plist_dict_get_item(plist, "$version"), &u);                       \
  ASSERT_EQ(100000, u);                                                                 \
                                                                                        \
  plist_t objects_node = plist_dict_get_item(plist, "$objects");                        \
  ASSERT_EQ(PLIST_ARRAY, plist_get_node_type(objects_node));

#define OBJECTS objects_node
#define OBJECTS_COUNT plist_array_get_size(objects_node)
#define GET_OBJECT(index) plist_array_get_item(objects_node, index)
#define ASSERT_PLIST_TYPE_EQ(node, type) ASSERT_EQ(plist_get_node_type(node), type)
#define ASSERT_PLIST_ARRAY_SIZE_EQ(node, size) ASSERT_EQ(plist_array_get_size(node), size);
#define GET_OBJECT_REF(uid_node)               \
  ({                                           \
    ASSERT_PLIST_TYPE_EQ(uid_node, PLIST_UID); \
    uint64_t uid = 0;                          \
    plist_get_uid_val(uid_node, &uid);         \
    GET_OBJECT(uid);                           \
  })

static inline std::string plist_get_std_string(plist_t node) {
  uint64_t length = 0;
  char* str = nullptr;
  plist_get_string_val(node, &str);
  std::string result = str != nullptr ? str : "";  // copy
  free(str);
  return result;
}

static inline bool plist_get_boolean(plist_t node, bool def_val = false) {
  uint8_t v = def_val;
  plist_get_bool_val(node, &v);
  return v;
}

static inline uint64_t plist_get_integer(plist_t node, uint64_t def_val = 0) {
  uint64_t v = def_val;
  plist_get_uint_val(node, &v);
  return v;
}

static inline double plist_get_double(plist_t node, double def_val = 0) {
  double v = def_val;
  plist_get_real_val(node, &v);
  return v;
}

/*
TEST(NSKeyedArchiverTest, EncodeBool_Xml) {
  const KAValue& object = KAValue(true);
  char* buffer = nullptr;
  size_t buffer_size = 0;
  NSKeyedArchiver::ArchivedData(object, &buffer, &buffer_size, NSKeyedArchiver::OutputFormat::Xml);
  ASSERT_TRUE(buffer != nullptr);
  ASSERT_TRUE(buffer_size > 0);
  printf("%s", buffer);

  plist_t plist;
  plist_from_xml(buffer, (uint32_t)buffer_size, &plist);
  plist_type node_type = plist_get_node_type(plist);
  ASSERT_EQ(PLIST_DICT, node_type);

  //print_plist_as_xml(plist);

  // clean up
  plist_free(plist);
  free(buffer);
}
*/

TEST(NSKeyedArchiverTest, EncodeNull) {
  const KAValue object;  // nullptr
  ASSERT_TRUE(object.IsNull());

  ARCHIVED_DATA(object);
  ASSERT_EQ(1, OBJECTS_COUNT);
  ASSERT_STREQ("$null", plist_get_std_string(GET_OBJECT(0)).c_str());
}

TEST(NSKeyedArchiverTest, EncodeBool) {
  const KAValue object(true);

  ARCHIVED_DATA(object);
  ASSERT_EQ(2, OBJECTS_COUNT);
  ASSERT_STREQ("$null", plist_get_std_string(GET_OBJECT(0)).c_str());
  ASSERT_EQ(1, plist_get_boolean(GET_OBJECT(1)));
}

TEST(NSKeyedArchiverTest, EncodeInteger) {
  uint64_t i = 3;
  const KAValue object(i);
  ASSERT_EQ(i, object.ToInteger());

  ARCHIVED_DATA(object);
  ASSERT_EQ(2, OBJECTS_COUNT);
  ASSERT_STREQ("$null", plist_get_std_string(GET_OBJECT(0)).c_str());
  ASSERT_EQ(i, plist_get_integer(GET_OBJECT(1)));
}

TEST(NSKeyedArchiverTest, EncodeDouble) {
  double d = 3.3;
  const KAValue object(d);
  ASSERT_EQ(d, object.ToDouble());

  ARCHIVED_DATA(object);
  ASSERT_EQ(2, OBJECTS_COUNT);
  ASSERT_STREQ("$null", plist_get_std_string(GET_OBJECT(0)).c_str());
  // NOTE: actual got: `<real>3.2999999999999998</real>`
  // see: https://github.com/libimobiledevice/libplist/issues/146
  ASSERT_EQ(d, plist_get_double(GET_OBJECT(1)));
}

TEST(NSKeyedArchiverTest, EncodeStr) {
  const char* s = "3.3";
  const KAValue object(s);
  ASSERT_STREQ(s, object.ToStr());

  ARCHIVED_DATA(object);
  ASSERT_EQ(2, OBJECTS_COUNT);
  ASSERT_STREQ("$null", plist_get_std_string(GET_OBJECT(0)).c_str());
  ASSERT_STREQ(s, plist_get_std_string(GET_OBJECT(1)).c_str());
}

TEST(NSKeyedArchiverTest, EncodeRaw) {
  char raw[] = {0x00, 0x01, 0x02, 0x03, 0x04};
  const KAValue object(KAValue::RawData{raw, sizeof(raw)});

  ARCHIVED_DATA(object);
  ASSERT_EQ(2, OBJECTS_COUNT);
  ASSERT_STREQ("$null", plist_get_std_string(GET_OBJECT(0)).c_str());

  char* data;
  uint64_t size;
  plist_get_data_val(GET_OBJECT(1), &data, &size);
  ASSERT_TRUE(memcmp(raw, data, size) == 0);
  free(data);
}

TEST(NSKeyedArchiverTest, EncodeObject_NSArray) {
  KAArray arr("NSArray", {"NSArray", "NSObject"}, {KAValue(3), KAValue(36079887892856llu)});
  const KAValue object(std::move(arr));

  ARCHIVED_DATA(object);
  ASSERT_EQ(5, OBJECTS_COUNT);
  ASSERT_STREQ("$null", plist_get_std_string(GET_OBJECT(0)).c_str());

  // root
  plist_t item_1 = GET_OBJECT(1);
  ASSERT_PLIST_TYPE_EQ(item_1, PLIST_DICT);

  // NS.objects
  plist_t ns_objects = plist_dict_get_item(item_1, "NS.objects");
  ASSERT_PLIST_ARRAY_SIZE_EQ(ns_objects, 2);

  // object #0
  plist_t object_0 = GET_OBJECT_REF(plist_array_get_item(ns_objects, 0));
  ASSERT_EQ(3, plist_get_integer(object_0));

  // object #1
  plist_t object_1 = GET_OBJECT_REF(plist_array_get_item(ns_objects, 1));
  ASSERT_EQ(36079887892856llu, plist_get_integer(object_1));

  // $class
  plist_t ns_class_ref = plist_dict_get_item(item_1, "$class");
  plist_t ns_class = GET_OBJECT_REF(ns_class_ref);
  ASSERT_STREQ("NSArray",
               plist_get_std_string(plist_dict_get_item(ns_class, "$classname")).c_str());
  plist_t classes = plist_dict_get_item(ns_class, "$classes");
  ASSERT_PLIST_TYPE_EQ(classes, PLIST_ARRAY);
  ASSERT_PLIST_ARRAY_SIZE_EQ(classes, 2);
  ASSERT_STREQ("NSArray", plist_get_std_string(plist_array_get_item(classes, 0)).c_str());
  ASSERT_STREQ("NSObject", plist_get_std_string(plist_array_get_item(classes, 1)).c_str());
}

TEST(NSKeyedArchiverTest, EncodeObject_NSDictionary) {
  KAMap map("NSMutableDictionary", {"NSMutableDictionary", "NSDictionary", "NSObject"},
            {
                {"Device Utilization %", KAValue(12)},
                {"Renderer Utilization %", KAValue(12)},
                {"Alloc system memory", KAValue(620331008)},
                {"recoveryCount", KAValue(1)},
                {"Allocated PB Size", KAValue(9961472)},
                {"Tiler Utilization %", KAValue(12)},
                {"SplitSceneCount", KAValue(0)},
                {"XRVideoCardRunTimeStamp", KAValue(10140644)},
                {"In use system memory", KAValue(179060736)},
                {"TiledSceneBytes", KAValue(524288)},
                {"IOGLBundleName", KAValue("Built-In")},
                {"CoreAnimationFramesPerSecond", KAValue(60)},
            });
  const KAValue object(std::move(map));

  ARCHIVED_DATA(object);
  ASSERT_EQ(25, OBJECTS_COUNT);
  ASSERT_STREQ("$null", plist_get_std_string(GET_OBJECT(0)).c_str());

  // root
  plist_t item_1 = GET_OBJECT(1);
  ASSERT_PLIST_TYPE_EQ(item_1, PLIST_DICT);

  // NS.keys
  plist_t ns_keys = plist_dict_get_item(item_1, "NS.keys");
  size_t key_count = plist_array_get_size(ns_keys);
  ASSERT_EQ(12, key_count);

  // NS.objects
  plist_t ns_objects = plist_dict_get_item(item_1, "NS.objects");
  size_t value_count = plist_array_get_size(ns_keys);
  ASSERT_EQ(12, value_count);

  // make sure all values are correct
  ASSERT_EQ(key_count, value_count);
  std::unordered_map<std::string, plist_t> encoded_map;
  for (int i = 0; i < key_count; ++i) {
    std::string key = plist_get_std_string(GET_OBJECT_REF(plist_array_get_item(ns_keys, i)));
    plist_t value_node = GET_OBJECT_REF(plist_array_get_item(ns_objects, i));
    encoded_map[key] = value_node;
  }
  ASSERT_EQ(plist_get_integer(encoded_map["Device Utilization %"]), 12);
  ASSERT_EQ(plist_get_integer(encoded_map["Renderer Utilization %"]), 12);
  ASSERT_EQ(plist_get_integer(encoded_map["Alloc system memory"]), 620331008);
  ASSERT_EQ(plist_get_integer(encoded_map["recoveryCount"]), 1);
  ASSERT_EQ(plist_get_integer(encoded_map["Allocated PB Size"]), 9961472);
  ASSERT_EQ(plist_get_integer(encoded_map["Tiler Utilization %"]), 12);
  ASSERT_EQ(plist_get_integer(encoded_map["SplitSceneCount"]), 0);
  ASSERT_EQ(plist_get_integer(encoded_map["XRVideoCardRunTimeStamp"]), 10140644);
  ASSERT_EQ(plist_get_integer(encoded_map["In use system memory"]), 179060736);
  ASSERT_EQ(plist_get_integer(encoded_map["TiledSceneBytes"]), 524288);
  ASSERT_STREQ(plist_get_std_string(encoded_map["IOGLBundleName"]).c_str(), "Built-In");
  ASSERT_EQ(plist_get_integer(encoded_map["CoreAnimationFramesPerSecond"]), 60);

  // $class
  plist_t ns_class_ref = plist_dict_get_item(item_1, "$class");
  plist_t ns_class = GET_OBJECT_REF(ns_class_ref);
  ASSERT_STREQ("NSMutableDictionary",
               plist_get_std_string(plist_dict_get_item(ns_class, "$classname")).c_str());
  plist_t classes = plist_dict_get_item(ns_class, "$classes");
  ASSERT_PLIST_TYPE_EQ(classes, PLIST_ARRAY);
  ASSERT_PLIST_ARRAY_SIZE_EQ(classes, 3);
  ASSERT_STREQ("NSMutableDictionary",
               plist_get_std_string(plist_array_get_item(classes, 0)).c_str());
  ASSERT_STREQ("NSDictionary", plist_get_std_string(plist_array_get_item(classes, 1)).c_str());
  ASSERT_STREQ("NSObject", plist_get_std_string(plist_array_get_item(classes, 2)).c_str());
}

TEST(NSKeyedArchiverTest, ObjectMap) {
  KAValueUIDMap<NSKeyedArchiverUID> map;  // KAValue(12) -> uid

  KAMap kamap("NSMutableDictionary", {"NSMutableDictionary", "NSDictionary", "NSObject"},
              {
                  {"Device Utilization %", KAValue(12)},
                  {"Renderer Utilization %", KAValue(12)},
                  {"Alloc system memory", KAValue(620331008)},
                  {"recoveryCount", KAValue(1)},
                  {"Allocated PB Size", KAValue(9961472)},
                  {"Tiler Utilization %", KAValue(12)},
                  {"SplitSceneCount", KAValue(0)},
                  {"XRVideoCardRunTimeStamp", KAValue(10140644)},
                  {"In use system memory", KAValue(179060736)},
                  {"TiledSceneBytes", KAValue(524288)},
                  {"IOGLBundleName", KAValue("Built-In")},
                  {"CoreAnimationFramesPerSecond", KAValue(60)},
              });
  const KAValue object(std::move(kamap));
  for (auto& it : object.AsObject<KAMap>().ToMap()) {
    // printf("key: %s\n", KAValue(it.first.c_str()).ToJson().c_str());
    map.Put(KAValue(it.first.c_str()), 0);
    // printf("value: %s\n", it.second.ToJson().c_str());
    map.Put(it.second, 0);
  }
  ASSERT_TRUE(map.Contains(KAValue(12)));
  ASSERT_FALSE(map.Contains(KAValue(13)));
}
