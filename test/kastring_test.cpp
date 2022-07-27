#include "nskeyedarchiver/kastring.hpp"

#include <gtest/gtest.h>
#include <sys/stat.h>

#include <cstring>

using namespace nskeyedarchiver;

TEST(KAStringTest, Ctor) {
  const char* str = "str";
  KAString obj("NSString", {"NSString", "NSObject"}, str);

  ASSERT_STREQ(str, obj.ToString().c_str());
  ASSERT_FALSE(obj.Empty());
  ASSERT_EQ(strlen(str), obj.Size());
}

TEST(KAStringTest, CopyCtor) {
  const char* str = "str";
  KAString obj("NSString", {"NSString", "NSObject"}, str);

  // copy constructor
  KAString cloned1(obj);
  ASSERT_STREQ(obj.ToString().c_str(), cloned1.ToString().c_str());
  ASSERT_EQ(obj.Size(), cloned1.Size());

  // copy assignment operator
  KAString cloned2 = obj;
  ASSERT_STREQ(obj.ToString().c_str(), cloned2.ToString().c_str());
  ASSERT_EQ(obj.Size(), cloned2.Size());
}

TEST(KAStringTest, MoveCtor) {
  const char* str = "str";
  KAString obj("NSString", {"NSString", "NSObject"}, str);

  // move constructor
  KAString obj1(std::move(obj));
  ASSERT_STREQ(str, obj1.ToString().c_str());
  ASSERT_TRUE(obj.Empty());  // the moved object has been reset

  // move assignment operator
  KAString obj2 = std::move(obj1);
  ASSERT_STREQ(str, obj2.ToString().c_str());
  ASSERT_TRUE(obj1.Empty());  // the moved object has been reset
}

TEST(KAStringTest, Equals) {
  KAString obj("NSString", {"NSString", "NSObject"}, "str");

  KAString same_class_same_content("NSString", {"NSString", "NSObject"}, "str");
  ASSERT_TRUE(same_class_same_content == obj);

  KAString same_class_diff_content("NSArray", {"NSArray", "NSObject"}, "str1");
  ASSERT_FALSE(same_class_diff_content == obj);

  KAString diff_class_same_content("NSMutableString", {"NSMutableString", "NSString", "NSObject"},
                                   "str");
  ASSERT_FALSE(diff_class_same_content == obj);
}
