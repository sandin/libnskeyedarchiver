#include "nskeyedarchiver/kaarray.hpp"

#include <gtest/gtest.h>
#include <sys/stat.h>
#include <vector>

using namespace nskeyedarchiver;

TEST(KAArrayTest, Ctor) {
  std::vector<KAValue> arr = {KAValue(0), KAValue(1), KAValue(2), KAValue(3)};
  KAArray obj("NSArray", {"NSArray", "NSObject"}, arr);
  
  ASSERT_FALSE(obj.Empty());
  ASSERT_EQ(arr.size(), obj.Size());
  ASSERT_TRUE(arr[0] == obj[0]);
  ASSERT_TRUE(arr[1] == obj[1]);
  ASSERT_TRUE(arr[2] == obj[2]);
  ASSERT_TRUE(arr[3] == obj[3]);
}

TEST(KAArrayTest, CopyCtor) {
  std::vector<KAValue> arr = {KAValue(0), KAValue(1), KAValue(2), KAValue(3)};
  KAArray obj("NSArray", {"NSArray", "NSObject"}, arr);
  
  // copy constructor
  KAArray cloned1(obj);
  ASSERT_TRUE(obj == cloned1);
  ASSERT_EQ(obj.Size(), cloned1.Size());
  
  // copy assignment operator
  KAArray cloned2 = obj;
  ASSERT_TRUE(obj == cloned2);
  ASSERT_EQ(obj.Size(), cloned2.Size());
}

TEST(KAArrayTest, MoveCtor) {
  std::vector<KAValue> arr = {KAValue(0), KAValue(1), KAValue(2), KAValue(3)};
  KAArray obj("NSArray", {"NSArray", "NSObject"}, arr);
  
  // copy constructor
  KAArray obj1(std::move(obj));
  ASSERT_FALSE(obj == obj1);
  ASSERT_EQ(4, obj1.Size());
  ASSERT_TRUE(obj.Empty()); // // the moved object has been reset
  
  // copy assignment operator
  KAArray obj2 = std::move(obj1);
  ASSERT_FALSE(obj1 == obj2);
  ASSERT_EQ(4, obj2.Size());
  ASSERT_TRUE(obj1.Empty()); // // the moved object has been reset
}

TEST(KAArrayTest, Equals) {
  KAArray obj("NSArray", {"NSArray", "NSObject"}, {KAValue(0), KAValue(1), KAValue(2), KAValue(3)});
  
  KAArray same_class_same_content("NSArray", {"NSArray", "NSObject"}, {KAValue(0), KAValue(1), KAValue(2), KAValue(3)});
  ASSERT_TRUE(same_class_same_content == obj);
  
  KAArray same_class_diff_content("NSArray", {"NSArray", "NSObject"}, {KAValue(3), KAValue(2), KAValue(1), KAValue(0)});
  ASSERT_FALSE(same_class_diff_content == obj);
  
  KAArray diff_class_same_content("NSMutableArray", {"NSMutableArray", "NSArray", "NSObject"}, {KAValue(0), KAValue(1), KAValue(2), KAValue(3)});
  ASSERT_FALSE(diff_class_same_content == obj);
}
