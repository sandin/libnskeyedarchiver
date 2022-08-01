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

TEST(NSKeyedArchiverTest, EncodeBool) {
  const KAValue& object = KAValue(true);
  char* buffer = nullptr;
  size_t buffer_size = 0;
  NSKeyedArchiver::ArchivedData(object, &buffer, &buffer_size,
                                NSKeyedArchiver::OutputFormat::Binary);
  ASSERT_TRUE(buffer != nullptr);
  ASSERT_TRUE(buffer_size > 0);

  plist_t plist;
  plist_from_bin(buffer, buffer_size, &plist);
  plist_type node_type = plist_get_node_type(plist);
  ASSERT_EQ(PLIST_DICT, node_type);
  print_plist_as_xml(plist);

  ASSERT_EQ(4, plist_dict_get_size(plist));

  uint64_t length;
  ASSERT_STREQ("NSKeyedArchiver",
               plist_get_string_ptr(plist_dict_get_item(plist, "$archiver"), &length));

  uint64_t u;
  plist_get_uint_val(plist_dict_get_item(plist, "$version"), &u);
  ASSERT_EQ(100000, u);

  plist_t objects_node = plist_dict_get_item(plist, "$objects");
  ASSERT_EQ(PLIST_ARRAY, plist_get_node_type(objects_node));
  plist_t null_object_node = plist_array_get_item(objects_node, 0);
  ASSERT_STREQ("$null", plist_get_string_ptr(null_object_node, &length));

  plist_t bool_object_node = plist_array_get_item(objects_node, 1);
  uint8_t b;
  plist_get_bool_val(bool_object_node, &b);
  ASSERT_EQ(1, b);

  // clean up
  plist_free(plist);
  free(buffer);
}
