#include "nskeyedarchiver/nstypes.hpp"

#include "nskeyedarchiver/kaarray.hpp"
#include "nskeyedarchiver/kamap.hpp"

using namespace nskeyedarchiver;

/* -- NSDummy -- */

// static
KAValue NSDummy::Deserialize(NSKeyedUnarchiver* decoder, const NSClass& clazz) {
  return KAValue(KAObject(KAObject::Kind::ObjectKind, clazz.class_name,
                          clazz.classes));  // we don't know how to parse this class of data yet
}

// static
bool NSDummy::Serialize(NSKeyedArchiver* encoder, const NSClass& clazz, const KAValue& object) {
  KAValue value("dummpy");
  encoder->EncodeObject(value, "dummpy");
  return true;
}

/* -- NSDictionary -- */

// static
KAValue NSDictionary::Deserialize(NSKeyedUnarchiver* decoder, const NSClass& clazz) {
  NSKEYEDARCHIVER_LOG_DEBUG("NSDictionary deserialize\n");
  if (!decoder->ContainsValue("NS.keys")) {
    NSKEYEDARCHIVER_LOG_ERROR("missing NS.keys.\n");
    return KAValue();  // null
  }
  if (!decoder->ContainsValue("NS.objects")) {
    NSKEYEDARCHIVER_LOG_ERROR("missing NS.objects.\n");
    return KAValue();  // null
  }

  std::vector<KAValue> keys = decoder->DecodeArrayOfObjectsForKey("NS.keys");
  std::vector<KAValue> values = decoder->DecodeArrayOfObjectsForKey("NS.objects");

  size_t keys_size = keys.size();
  size_t values_size = values.size();
  if (keys_size != values_size) {
    NSKEYEDARCHIVER_LOG_ERROR("values size(%lu) and keys size(%lu) are not equal.\n", values_size, keys_size);
    return KAValue();  // null
  }

  KAMap map(clazz.class_name, clazz.classes);
  int i = 0;
  for (const KAValue& key : keys) {
    if (key.IsStr()) {
      map[key.ToStr()] = values[i];
    } else if (key.IsInteger()) {
      // some NSDictionary use the integer type index as the key, e.g.: 0, 1, 2, 3
      // we simplify the logic by converting it to a string
      map[std::to_string(key.ToInteger())] = values[i];
    } else {
      // actually, any KAValue can be used as a key, we just haven't implemented it yet.
      NSKEYEDARCHIVER_LOG_ERROR(
          "we only support string/integer type keys for now, unsupport type: %d, index: %d key: "
          "%s.\n",
          key.GetDataType(), i, key.ToJson().c_str());
      return KAValue();  // null
    }
    i++;
  }

  return KAValue(map);
}

// static
bool NSDictionary::Serialize(NSKeyedArchiver* encoder, const NSClass& clazz,
                             const KAValue& object) {
  NSKEYEDARCHIVER_LOG_DEBUG("NSDictionary serialize\n");
  const KAObject* obj = object.ToObject();
  if (!obj->IsA(KAObject::Kind::MapKind)) {
    NSKEYEDARCHIVER_LOG_ERROR("This KAObject is not a KAMap.\n");
    return false;
  }

  const KAMap& kamap = object.AsObject<KAMap>();
  const KAMap::ObjectMap& map = kamap.ToMap();
  std::vector<KAValue> keys;
  std::vector<std::reference_wrapper<const KAValue>> values;
  for (auto& it : map) {
    keys.emplace_back(KAValue(it.first.c_str()));  // TODO: int type key?
    values.emplace_back(it.second);
  }

  encoder->EncodeArrayOfObjects(keys, "NS.keys");
  encoder->EncodeArrayOfObjectRefs(values, "NS.objects");
  return true;
}

/* -- NSArray -- */

// static
KAValue NSArray::Deserialize(NSKeyedUnarchiver* decoder, const NSClass& clazz) {
  NSKEYEDARCHIVER_LOG_DEBUG("NSArray deserialize\n");
  if (!decoder->ContainsValue("NS.objects")) {
    NSKEYEDARCHIVER_LOG_ERROR("missing NS.objects.\n");
    return KAValue();  // null
  }

  std::vector<KAValue> values = decoder->DecodeArrayOfObjectsForKey("NS.objects");
  KAArray arr(clazz.class_name, clazz.classes, std::move(values));
  return KAValue(arr);
}

// static
bool NSArray::Serialize(NSKeyedArchiver* encoder, const NSClass& clazz, const KAValue& object) {
  NSKEYEDARCHIVER_LOG_DEBUG("NSArray serialize\n");
  const KAObject* obj = object.ToObject();
  if (!obj->IsA(KAObject::Kind::ArrayKind)) {
    NSKEYEDARCHIVER_LOG_ERROR("This KAObject is not a KAArray.\n");
    return false;
  }

  const KAArray& array = object.AsObject<KAArray>();
  encoder->EncodeArrayOfObjects(array.ToArray(), "NS.objects");
  return true;
}

/* -- NSString -- */

// static
bool NSString::Serialize(NSKeyedArchiver* encoder, const NSClass& clazz, const KAValue& object) {
  NSKEYEDARCHIVER_LOG_DEBUG("NSString serialize\n");
  const KAObject* obj = object.ToObject();
  if (!obj->IsA(KAObject::Kind::MapKind)) {
    NSKEYEDARCHIVER_LOG_ERROR("This KAObject is not a KAMap.\n");
    return false;
  }

  const KAMap& kamap = object.AsObject<KAMap>();
  const KAMap::ObjectMap& map = kamap.ToMap();
  encoder->EncodeObject(map.at("NS.string"), "NS.string");
  return true;
}

// static
KAValue NSString::Deserialize(NSKeyedUnarchiver* decoder, const NSClass& clazz) {
  NSKEYEDARCHIVER_LOG_DEBUG("NSString deserialize\n");
  if (!decoder->ContainsValue("NS.string")) {
    NSKEYEDARCHIVER_LOG_ERROR("missing NS.string.\n");
    return KAValue();  // null
  }

  std::string s = decoder->DecodeString("NS.string");
  KAMap fields(clazz.class_name, clazz.classes, {{"string", KAValue(s.c_str())}});
  return KAValue(fields);
}

/* -- NSDate -- */

// static
bool NSDate::Serialize(NSKeyedArchiver* encoder, const NSClass& clazz, const KAValue& object) {
  return false; // TODO
}

// static
KAValue NSDate::Deserialize(NSKeyedUnarchiver* decoder, const NSClass& clazz) {
  NSKEYEDARCHIVER_LOG_DEBUG("NSDate deserialize\n");
  if (!decoder->ContainsValue("NS.time")) {
    NSKEYEDARCHIVER_LOG_ERROR("missing NS.time.\n");
    return KAValue();  // null
  }

  double ti = decoder->DecodeDouble("NS.time");
  KAMap fields(
      clazz.class_name, clazz.classes,
      {{"time",
        KAValue(std::to_string(ti + 978307200 /* 00:00:00 UTC on 1 January 2001 */).c_str())}});
  return KAValue(fields);
}

/* -- NSNull -- */

// static
bool NSNull::Serialize(NSKeyedArchiver* encoder, const NSClass& clazz, const KAValue& object) {
  return false; // TODO
}

// static
KAValue NSNull::Deserialize(NSKeyedUnarchiver* decoder, const NSClass& clazz) {
  NSKEYEDARCHIVER_LOG_DEBUG("NSNull deserialize\n");
  return KAValue();  // null
}

/* -- NSError -- */

// static
bool NSError::Serialize(NSKeyedArchiver* encoder, const NSClass& clazz, const KAValue& object) {
  return false; // TODO
}

// static
KAValue NSError::Deserialize(NSKeyedUnarchiver* decoder, const NSClass& clazz) {
  NSKEYEDARCHIVER_LOG_DEBUG("NSError deserialize\n");

  uint64_t code = decoder->DecodeInteger("NSCode");
  KAValue domain = decoder->DecodeObject("NSDomain");
  KAValue user_info = decoder->DecodeObject("NSUserInfo");

  KAMap fields(clazz.class_name, clazz.classes,
               {
                   {"code", KAValue(code)},
                   {"domain", std::move(domain)},
                   {"userInfo", std::move(user_info)},
               });
  return KAValue(fields);
}

/* -- DTSysmonTapMessage -- */

// static
bool DTSysmonTapMessage::Serialize(NSKeyedArchiver* encoder, const NSClass& clazz, const KAValue& object) {
  return false; // TODO
}

// static
KAValue DTSysmonTapMessage::Deserialize(NSKeyedUnarchiver* decoder, const NSClass& clazz) {
  NSKEYEDARCHIVER_LOG_DEBUG("DTSysmonTapMessage deserialize\n");

  KAValue plist = decoder->DecodeObject("DTTapMessagePlist");
  KAMap fields(clazz.class_name, clazz.classes,
               {
                   {"plist", std::move(plist)},
               });
  return KAValue(fields);
}
