#include "nskeyedarchiver/kamap.hpp"

#include <gtest/gtest.h>
#include <sys/stat.h>

#include <map>
#include <string>

using namespace nskeyedarchiver;

TEST(KAMapTest, Ctor) {
  std::map<std::string, KAValue> map = {
      {"key1", KAValue("value1")},
      {"key2", KAValue("value2")},
  };
  KAMap obj("NSDictionary", {"NSDictionary", "NSObject"}, map);

  ASSERT_FALSE(obj.Empty());
  ASSERT_EQ(map.size(), obj.Size());
  ASSERT_TRUE(map["key1"] == obj["key1"]);
  ASSERT_TRUE(map["key2"] == obj["key2"]);
}

TEST(KAMapTest, CopyCtor) {
  std::map<std::string, KAValue> map = {
      {"key1", KAValue("value1")},
      {"key2", KAValue("value2")},
  };
  KAMap obj("NSDictionary", {"NSDictionary", "NSObject"}, map);

  // copy constructor
  KAMap cloned1(obj);
  ASSERT_TRUE(obj == cloned1);
  ASSERT_EQ(obj.Size(), cloned1.Size());

  // copy assignment operator
  KAMap cloned2 = obj;
  ASSERT_TRUE(obj == cloned2);
  ASSERT_EQ(obj.Size(), cloned2.Size());
}

TEST(KAMapTest, MoveCtor) {
  std::map<std::string, KAValue> map = {
      {"key1", KAValue("value1")},
      {"key2", KAValue("value2")},
  };
  KAMap obj("NSDictionary", {"NSDictionary", "NSObject"}, map);

  // copy constructor
  KAMap obj1(std::move(obj));
  ASSERT_FALSE(obj == obj1);
  ASSERT_EQ(2, obj1.Size());
  ASSERT_TRUE(obj.Empty());  // // the moved object has been reset

  // copy assignment operator
  KAMap obj2 = std::move(obj1);
  ASSERT_FALSE(obj1 == obj2);
  ASSERT_EQ(2, obj2.Size());
  ASSERT_TRUE(obj1.Empty());  // // the moved object has been reset
}

TEST(KAMapTest, Equals) {
  KAMap obj("NSDictionary", {"NSDictionary", "NSObject"},
            {
                {"key1", KAValue("value1")},
                {"key2", KAValue("value2")},
            });

  KAMap same_class_same_content("NSDictionary", {"NSDictionary", "NSObject"},
                                {
                                    {"key1", KAValue("value1")},
                                    {"key2", KAValue("value2")},
                                });
  ASSERT_TRUE(same_class_same_content == obj);

  KAMap same_class_diff_content("NSDictionary", {"NSDictionary", "NSObject"},
                                {
                                    {"key3", KAValue("value3")},
                                    {"key4", KAValue("value4")},
                                });
  ASSERT_FALSE(same_class_diff_content == obj);

  KAMap diff_class_same_content("NSMutableDictionary",
                                {"NSMutableDictionary", "NSDictionary", "NSObject"},
                                {
                                    {"key1", KAValue("value1")},
                                    {"key2", KAValue("value2")},
                                });
  ASSERT_FALSE(diff_class_same_content == obj);
}
