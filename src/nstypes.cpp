#include "nskeyedarchiver/nstypes.hpp"

#include "nskeyedarchiver/kaarray.hpp"
#include "nskeyedarchiver/kamap.hpp"
#include "nskeyedarchiver/kastring.hpp"

using namespace nskeyedarchiver;

/* -- NSDummy -- */

// static
KAValue NSDummy::Deserialize(NSKeyedUnarchiver* decoder, const NSClass& clazz) {
  return KAValue(KAObject(clazz.class_name,
                          clazz.classes));  // we don't know how to parse this class of data yet
}

/* -- NSDictionary -- */

// static
KAValue NSDictionary::Deserialize(NSKeyedUnarchiver* decoder, const NSClass& clazz) {
  LOG_INFO("NSDictionary deserialize\n");
  if (!decoder->ContainsValue("NS.keys")) {
    LOG_ERROR("missing NS.keys.\n");
    return KAValue();  // null
  }
  if (!decoder->ContainsValue("NS.objects")) {
    LOG_ERROR("missing NS.objects.\n");
    return KAValue();  // null
  }

  std::vector<KAValue> keys = decoder->DecodeArrayOfObjectsForKey("NS.keys");
  std::vector<KAValue> values = decoder->DecodeArrayOfObjectsForKey("NS.objects");

  size_t keys_size = keys.size();
  size_t values_size = values.size();
  if (keys_size != values_size) {
    LOG_ERROR("values size(%lu) and keys size(%lu) are not equal.\n", values_size, keys_size);
    return KAValue();  // null
  }

  KAMap map(clazz.class_name, clazz.classes);
  int i = 0;
  for (const KAValue& key : keys) {
    if (!key.IsStr()) {
      LOG_ERROR("we only support string type keys for now, unsupport type: %d.\n",
                key.GetDataType());
      return KAValue();  // null
    }
    map[key.ToStr()] = values[i];
    i++;
  }

  return KAValue(map);
}

/* -- NSArray -- */

// static
KAValue NSArray::Deserialize(NSKeyedUnarchiver* decoder, const NSClass& clazz) {
  LOG_INFO("NSArray deserialize\n");
  if (!decoder->ContainsValue("NS.objects")) {
    LOG_ERROR("missing NS.objects.\n");
    return KAValue();  // null
  }

  std::vector<KAValue> values = decoder->DecodeArrayOfObjectsForKey("NS.objects");
  KAArray arr(clazz.class_name, clazz.classes, std::move(values));
  return KAValue(arr);
}

/* -- NSString -- */

// static
KAValue NSString::Deserialize(NSKeyedUnarchiver* decoder, const NSClass& clazz) {
  LOG_INFO("NSString deserialize\n");
  if (!decoder->ContainsValue("NS.string")) {
    LOG_ERROR("missing NS.string.\n");
    return KAValue();  // null
  }

  std::string s = decoder->DecodeString("NS.string");
  KAString str(clazz.class_name, clazz.classes, std::move(s));
  return KAValue(str);
}

/* -- NSDate -- */

// static
KAValue NSDate::Deserialize(NSKeyedUnarchiver* decoder, const NSClass& clazz) {
  LOG_INFO("NSDate deserialize\n");
  if (!decoder->ContainsValue("NS.time")) {
    LOG_ERROR("missing NS.time.\n");
    return KAValue();  // null
  }
  
  double ti = decoder->DecodeDouble("NS.time");
  KAMap fields(clazz.class_name, clazz.classes, {
    {"time", KAValue(std::to_string(ti + 978307200 /* 00:00:00 UTC on 1 January 2001 */).c_str())}
  });
  return KAValue(fields);
}

/* -- NSNull -- */

// static
KAValue NSNull::Deserialize(NSKeyedUnarchiver* decoder, const NSClass& clazz) {
  LOG_INFO("NSNull deserialize\n");
  return KAValue();  // null
}

/* -- NSError -- */

// static
KAValue NSError::Deserialize(NSKeyedUnarchiver* decoder, const NSClass& clazz) {
  LOG_INFO("NSError deserialize\n");
  
  uint64_t code = decoder->DecodeInteger("NSCode");
  KAValue domain = decoder->DecodeObject("NSDomain");
  KAValue user_info = decoder->DecodeObject("NSUserInfo");
  
  KAMap fields(clazz.class_name, clazz.classes, {
    {"code", KAValue(code)},
    {"domain", domain},
    {"userInfo", std::move(user_info)},
  });
  return KAValue(fields);
}
