#include "nskeyedarchiver/nsdictionary.hpp"

#include <sstream>

#include "common.h"
#include "nskeyedarchiver/nskeyedunarchiver.hpp"

using namespace nskeyedarchiver;

bool NSDictionary::Decode(NSKeyedUnarchiver* decoder) {
  if (!decoder->ContainsValue("NS.keys")) {
    LOG_ERROR("missing NS.keys.\n");
    return false;
  }
  if (!decoder->ContainsValue("NS.objects")) {
    LOG_ERROR("missing NS.objects.\n");
    return false;
  }

  std::vector<NSObject*> keys = decoder->DecodeArrayOfObjectsForKey("NS.keys");
  std::vector<NSObject*> values =
      decoder->DecodeArrayOfObjectsForKey("NS.objects");

  size_t keys_size = keys.size();
  size_t values_size = values.size();
  if (keys_size != values_size) {
    LOG_ERROR("values size(%lu) and keys size(%lu) are not equal.\n", values_size,
              keys_size);
    return false;
  }

  for (int i = 0; i < keys_size; ++i) {
    NSObject* key = keys[i];
    NSObject* value = values[i];
    // if (key != nullptr) { // null is not allowed
    items_[key] = value;
    //}
  }

  return true;
}

std::string NSDictionary::ToString() const {
  std::stringstream ss;
  ss << "[NSDictionary, length=" << items_.size() << ", items=[";
  int i = 0;
  int size = items_.size();
  for (const auto& item : items_) {
    ss << item.first->ToString() << " : " << item.second->ToString();
    if (i < size - 1) {
      ss << ", ";
    }
    i++;
  }
  ss << "]" << std::endl;
  return ss.str();
}