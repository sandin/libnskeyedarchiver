#include "nskeyedarchiver/nskeyedarchiver.hpp"

#include <gtest/gtest.h>
#include <sys/stat.h>

#include <string>

#include "nskeyedarchiver/common.hpp"
#include "nskeyedarchiver/kaarray.hpp"
#include "nskeyedarchiver/kamap.hpp"
#include "nskeyedarchiver/kastring.hpp"
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

static inline std::string plist_get_std_string(plist_t node) {
  uint64_t length = 0;
  char* str = nullptr;
  plist_get_string_val(node, &str);
  std::string result = str;  // copy
  free(str);
  return result;
}

static inline bool plist_get_boolean(plist_t node) {
  uint8_t v;
  plist_get_bool_val(node, &v);
  return v;
}

static inline uint64_t plist_get_integer(plist_t node) {
  uint64_t v;
  plist_get_uint_val(node, &v);
  return v;
}

static inline double plist_get_double(plist_t node) {
  double v;
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
  const KAValue& object = KAValue();  // nullptr
  ASSERT_TRUE(object.IsNull());

  ARCHIVED_DATA(object);
  ASSERT_EQ(1, OBJECTS_COUNT);
  ASSERT_STREQ("$null", plist_get_std_string(GET_OBJECT(0)).c_str());
}

TEST(NSKeyedArchiverTest, EncodeBool) {
  const KAValue& object = KAValue(true);

  ARCHIVED_DATA(object);
  ASSERT_EQ(2, OBJECTS_COUNT);
  ASSERT_STREQ("$null", plist_get_std_string(GET_OBJECT(0)).c_str());
  ASSERT_EQ(1, plist_get_boolean(GET_OBJECT(1)));
}

TEST(NSKeyedArchiverTest, EncodeInteger) {
  uint64_t i = 3;
  const KAValue& object = KAValue(i);
  ASSERT_EQ(i, object.ToInteger());

  ARCHIVED_DATA(object);
  ASSERT_EQ(2, OBJECTS_COUNT);
  ASSERT_STREQ("$null", plist_get_std_string(GET_OBJECT(0)).c_str());
  ASSERT_EQ(i, plist_get_integer(GET_OBJECT(1)));
}

TEST(NSKeyedArchiverTest, EncodeDouble) {
  double d = 3.3;
  const KAValue& object = KAValue(d);
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
  const KAValue& object = KAValue(s);
  ASSERT_STREQ(s, object.ToStr());

  ARCHIVED_DATA(object);
  ASSERT_EQ(2, OBJECTS_COUNT);
  ASSERT_STREQ("$null", plist_get_std_string(GET_OBJECT(0)).c_str());
  ASSERT_STREQ(s, plist_get_std_string(GET_OBJECT(1)).c_str());
}

TEST(NSKeyedArchiverTest, EncodeRaw) {
  char raw[] = {0x00, 0x01, 0x02, 0x03, 0x04};
  const KAValue& object = KAValue(KAValue::RawData{raw, sizeof(raw)});

  ARCHIVED_DATA(object);
  ASSERT_EQ(2, OBJECTS_COUNT);
  ASSERT_STREQ("$null", plist_get_std_string(GET_OBJECT(0)).c_str());

  char* data;
  uint64_t size;
  plist_get_data_val(GET_OBJECT(1), &data, &size);
  ASSERT_TRUE(memcmp(raw, data, size) == 0);
  free(data);
}

TEST(NSKeyedArchiverTest, EncodeObject_KAArray) {
  KAArray arr("NSArray", {"NSArray", "NSObject"}, {KAValue(3), KAValue(36079887892856llu)});
  const KAValue& object = KAValue(std::move(arr));

  ARCHIVED_DATA(object);
  ASSERT_EQ(3, OBJECTS_COUNT);
  ASSERT_STREQ("$null", plist_get_std_string(GET_OBJECT(0)).c_str());
  // TODO
}
