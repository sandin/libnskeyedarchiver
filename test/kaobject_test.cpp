#include "nskeyedarchiver/kaobject.hpp"

#include <gtest/gtest.h>
#include <sys/stat.h>

using namespace nskeyedarchiver;

TEST(KAObjectTest, Ctor) {
  KAObject obj("NSString", {"NSString", "NSObject"});
  ASSERT_STREQ("NSString", obj.ClassName().c_str());
  ASSERT_EQ(2, obj.Classes().size());
  ASSERT_STREQ("NSString", obj.Classes()[0].c_str());
  ASSERT_STREQ("NSObject", obj.Classes()[1].c_str());
}

TEST(KAObjectTest, CopyCtor) {
  KAObject obj("NSString", {"NSString", "NSObject"});
  ASSERT_STREQ("NSString", obj.ClassName().c_str());
  ASSERT_EQ(2, obj.Classes().size());

  // copy constructor
  KAObject cloned1(obj);
  ASSERT_STREQ("NSString", cloned1.ClassName().c_str());
  ASSERT_TRUE(obj == cloned1);

  // copy assignment operator
  KAObject cloned2 = obj;
  ASSERT_STREQ("NSString", cloned2.ClassName().c_str());
  ASSERT_TRUE(obj == cloned2);
}

TEST(KAObjectTest, MoveCtor) {
  KAObject obj("NSString", {"NSString", "NSObject"});
  ASSERT_STREQ("NSString", obj.ClassName().c_str());
  ASSERT_EQ(2, obj.Classes().size());

  // move constructor
  KAObject obj1(std::move(obj));
  ASSERT_STREQ("NSString", obj1.ClassName().c_str());
  ASSERT_EQ(2, obj1.Classes().size());
  // the moved object has been reset
  ASSERT_FALSE(obj1 == obj);
  ASSERT_TRUE(obj.ClassName().empty());
  ASSERT_EQ(0, obj.Classes().size());

  // move assignment operator
  KAObject obj2 = std::move(obj1);
  ASSERT_STREQ("NSString", obj2.ClassName().c_str());
  ASSERT_EQ(2, obj2.Classes().size());
  // the moved object has been reset
  ASSERT_FALSE(obj2 == obj1);
  ASSERT_TRUE(obj1.ClassName().empty());
  ASSERT_EQ(0, obj1.Classes().size());
}

TEST(KAObjectTest, IsA) {
  KAObject obj("NSString", {"NSString", "NSObject"});
  ASSERT_TRUE(obj.IsA("NSString"));

  ASSERT_FALSE(obj.IsA("NSObject"));
  ASSERT_FALSE(obj.IsA("NSArray"));
  ASSERT_FALSE(obj.IsA("NSMap"));
}

TEST(KAObjectTest, IsInstanceOf) {
  KAObject obj("NSMutableString", {"NSMutableString", "NSString", "NSObject"});
  ASSERT_TRUE(obj.IsInstanceOf("NSMutableString"));
  ASSERT_TRUE(obj.IsInstanceOf("NSString"));
  ASSERT_TRUE(obj.IsInstanceOf("NSObject"));

  ASSERT_FALSE(obj.IsInstanceOf("NSArray"));
  ASSERT_FALSE(obj.IsInstanceOf("NSMap"));
}

TEST(KAObjectTest, Equals) {
  KAObject obj1("NSString", {"NSString", "NSObject"});

  KAObject obj2("NSString", {"NSString", "NSObject"});
  ASSERT_TRUE(obj1 == obj2);

  KAObject obj3("NSArray", {"NSArray", "NSObject"});
  ASSERT_FALSE(obj1 == obj3);

  KAObject obj4("NSMutableString", {"NSMutableString", "NSString", "NSObject"});
  ASSERT_FALSE(obj1 == obj4);
}
