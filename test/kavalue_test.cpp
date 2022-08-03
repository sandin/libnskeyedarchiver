#include "nskeyedarchiver/kavalue.hpp"

#include <gtest/gtest.h>
#include <sys/stat.h>

#include <map>

#include "nskeyedarchiver/kaobject.hpp"

using namespace nskeyedarchiver;

TEST(KAValueTest, Ctor_Integer_uint64_t) {
  // ctor
  uint64_t v = 3;
  KAValue value(v);
  ASSERT_TRUE(value.IsInteger());
  ASSERT_EQ(v, value.ToInteger());

  // assignment
  KAValue value2;
  value2 = v;
  ASSERT_TRUE(value2.IsInteger());
  ASSERT_EQ(v, value2.ToInteger());
}

TEST(KAValueTest, Ctor_Integer_uint32_t) {
  // ctor
  uint32_t v = 3;
  KAValue value(v);
  ASSERT_TRUE(value.IsInteger());
  ASSERT_EQ(v, value.ToInteger());

  // assignment
  KAValue value2;
  value2 = v;
  ASSERT_TRUE(value2.IsInteger());
  ASSERT_EQ(v, value2.ToInteger());
}

TEST(KAValueTest, Ctor_Integer_int64_t) {
  // ctor
  int64_t v = -3;
  KAValue value(v);
  ASSERT_TRUE(value.IsInteger());
  ASSERT_EQ(v, value.ToInteger());

  // assignment
  KAValue value2;
  value2 = v;
  ASSERT_TRUE(value2.IsInteger());
  ASSERT_EQ(v, value2.ToInteger());
}

TEST(KAValueTest, Ctor_Integer_int32_t) {
  // ctor
  int32_t v = -3;
  KAValue value(v);
  ASSERT_TRUE(value.IsInteger());
  ASSERT_EQ(v, value.ToInteger());

  // assignment
  KAValue value2;
  value2 = v;
  ASSERT_TRUE(value2.IsInteger());
  ASSERT_EQ(v, value2.ToInteger());
}

TEST(KAValueTest, Ctor_Bool) {
  // ctor
  bool b = 3;
  KAValue value(b);
  ASSERT_TRUE(value.IsBool());
  ASSERT_EQ(b, value.ToBool());

  // assignment
  KAValue value2;
  value2 = b;
  ASSERT_TRUE(value2.IsBool());
  ASSERT_EQ(b, value2.ToBool());
}

TEST(KAValueTest, Ctor_Double) {
  // ctor
  double d = 3.33;
  KAValue value(d);
  ASSERT_TRUE(value.IsDouble());
  ASSERT_EQ(d, value.ToDouble());

  // assignment
  KAValue value2;
  value2 = d;
  ASSERT_TRUE(value2.IsDouble());
  ASSERT_EQ(d, value2.ToDouble());
}

TEST(KAValueTest, Ctor_Null) {
  KAValue value;
  ASSERT_TRUE(value.IsNull());
}

TEST(KAValueTest, Ctor_Str) {
  // ctor
  const char* s = "3";
  KAValue value(s);
  ASSERT_TRUE(value.IsStr());
  ASSERT_STREQ(s, value.ToStr());

  // assignment
  KAValue value2;
  value2 = s;
  ASSERT_TRUE(value2.IsStr());
  ASSERT_STREQ(s, value2.ToStr());

  // copy ctor
  KAValue cloned1(value);
  ASSERT_TRUE(cloned1.IsStr());
  ASSERT_STREQ(s, cloned1.ToStr());
  ASSERT_TRUE(value.IsStr());  // untouched

  // move ctor
  KAValue value3(std::move(value2));
  ASSERT_TRUE(value3.IsStr());
  ASSERT_STREQ(s, value3.ToStr());
  ASSERT_TRUE(value2.IsNull());  // reset the moved value
}

TEST(KAValueTest, Ctor_Object) {
  // copy
  KAObject obj("NSObject", {"NSObject"});
  KAValue value(obj);
  ASSERT_TRUE(value.IsObject());
  ASSERT_STREQ(obj.ClassName().c_str(), value.AsObject<KAObject>().ClassName().c_str());

  /*
  // copy assignment
  KAValue value1;
  value1 = obj;
  ASSERT_TRUE(value1.IsObject());
  ASSERT_STREQ(obj.ClassName().c_str(), value1.AsObject<KAObject>().ClassName().c_str());
  */
}

TEST(KAValueTest, CopyAndMoveCtor_uint64_t) {
  // ctor
  uint64_t v = 3;
  KAValue value(v);

  // copy ctor
  KAValue cloned1(value);
  ASSERT_TRUE(cloned1.IsInteger());
  ASSERT_EQ(v, cloned1.ToInteger());
  ASSERT_TRUE(value.IsInteger());  // untouched

  // copy assignment operator
  KAValue cloned2 = value;
  ASSERT_TRUE(cloned2.IsInteger());
  ASSERT_EQ(v, cloned2.ToInteger());
  ASSERT_TRUE(value.IsInteger());  // untouched

  // move ctor
  KAValue value2(std::move(value));
  ASSERT_TRUE(value2.IsInteger());
  ASSERT_EQ(v, value2.ToInteger());
  ASSERT_TRUE(value.IsNull());  // reset the moved value

  // move assignment operator
  KAValue value3 = std::move(value2);
  ASSERT_TRUE(value3.IsInteger());
  ASSERT_EQ(v, value3.ToInteger());
  ASSERT_TRUE(value2.IsNull());  // reset the moved value
}

TEST(KAValueTest, CopyAndMoveCtor_Str) {
  // ctor
  const char* v = "3";
  KAValue value(v);

  // copy ctor
  KAValue cloned1(value);
  ASSERT_TRUE(cloned1.IsStr());
  ASSERT_STREQ(v, cloned1.ToStr());
  ASSERT_TRUE(value.IsStr());  // untouched

  // copy assignment operator
  KAValue cloned2 = value;
  ASSERT_TRUE(cloned2.IsStr());
  ASSERT_STREQ(v, cloned2.ToStr());
  ASSERT_TRUE(value.IsStr());  // untouched

  // move ctor
  KAValue value2(std::move(value));
  ASSERT_TRUE(value2.IsStr());
  ASSERT_STREQ(v, value2.ToStr());
  ASSERT_TRUE(value.IsNull());  // reset the moved value

  // move assignment operator
  KAValue value3 = std::move(value2);
  ASSERT_TRUE(value3.IsStr());
  ASSERT_STREQ(v, value3.ToStr());
  ASSERT_TRUE(value2.IsNull());  // reset the moved value
}

TEST(KAValueTest, CopyAndMoveCtor_Object) {
  // ctor
  KAObject obj("NSObject", {"NSObject"});
  KAValue value(obj);

  // copy ctor
  KAValue cloned1(value);
  ASSERT_TRUE(cloned1.IsObject());
  ASSERT_STREQ(obj.ClassName().c_str(), cloned1.AsObject<KAObject>().ClassName().c_str());
  ASSERT_TRUE(value.IsObject());  // untouched

  // copy assignment operator
  KAValue cloned2 = value;
  ASSERT_TRUE(cloned2.IsObject());
  ASSERT_STREQ(obj.ClassName().c_str(), cloned2.AsObject<KAObject>().ClassName().c_str());
  ASSERT_TRUE(value.IsObject());  // untouched

  // move ctor
  KAValue value2(std::move(value));
  ASSERT_TRUE(value2.IsObject());
  ASSERT_STREQ(obj.ClassName().c_str(), value2.AsObject<KAObject>().ClassName().c_str());
  ASSERT_TRUE(value.IsNull());  // reset the moved value

  // move assignment operator
  KAValue value3 = std::move(value2);
  ASSERT_TRUE(value3.IsObject());
  ASSERT_STREQ(obj.ClassName().c_str(), value3.AsObject<KAObject>().ClassName().c_str());
  ASSERT_TRUE(value2.IsNull());  // reset the moved value
}

TEST(KAValueTest, Equals) {
  ASSERT_TRUE(KAValue(1) == KAValue(1));
  ASSERT_FALSE(KAValue(1) == KAValue(2));
  ASSERT_FALSE(KAValue(1) == KAValue());

  ASSERT_TRUE(KAValue(true) == KAValue(true));
  ASSERT_FALSE(KAValue(true) == KAValue(false));
  ASSERT_FALSE(KAValue(true) == KAValue());

  ASSERT_TRUE(KAValue(1.1) == KAValue(1.1));
  ASSERT_FALSE(KAValue(1.1) == KAValue(1.2));
  ASSERT_FALSE(KAValue(1.1) == KAValue());

  ASSERT_TRUE(KAValue("abc") == KAValue("abc"));
  ASSERT_FALSE(KAValue("abc") == KAValue("xyz"));
  ASSERT_FALSE(KAValue("abc") == KAValue());

  ASSERT_TRUE(KAValue(KAObject("NSString", {"NSString", "NSObject"})) ==
              KAValue(KAObject("NSString", {"NSString", "NSObject"})));
  ASSERT_FALSE(KAValue(KAObject("NSString", {"NSString", "NSObject"})) ==
               KAValue(KAObject("NSArray", {"NSArray", "NSObject"})));
  ASSERT_FALSE(KAValue(KAObject("NSString", {"NSString", "NSObject"})) == KAValue());
}

TEST(KAValueTest, KAValueComparator) {
  std::map<KAValue, int, KAValueComparator> map;
  KAValue not_exists_key("not exists");

  KAValue null_as_key;
  map[null_as_key] = 0;
  ASSERT_TRUE(map.find(null_as_key) != map.end());
  ASSERT_TRUE(map.find(KAValue()) != map.end());
  ASSERT_FALSE(map.find(not_exists_key) != map.end());
  map.clear();

  KAValue str_as_key("a");
  map[str_as_key] = 0;
  ASSERT_TRUE(map.find(str_as_key) != map.end());
  ASSERT_TRUE(map.find(KAValue("a")) != map.end());
  ASSERT_FALSE(map.find(not_exists_key) != map.end());
  map.clear();

  KAValue int_as_key(3);
  map[int_as_key] = 0;
  ASSERT_TRUE(map.find(int_as_key) != map.end());
  ASSERT_TRUE(map.find(KAValue(3)) != map.end());
  ASSERT_FALSE(map.find(not_exists_key) != map.end());
  map.clear();

  // TODO: test other type as key
}
