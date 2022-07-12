#include "nskeyedarchiver/nsset.hpp"

#include <sstream>

#include "nskeyedarchiver/nskeyedunarchiver.hpp"
#include "common.h"

using namespace nskeyedarchiver;

bool NSSet::Decode(NSKeyedUnarchiver* decoder) {
  if (!decoder->ContainsValue("NS.objects")) {
    LOG_ERROR("NSDictionary, missing NS.objects.\n");
    return false;
  }

  std::vector<NSObject*> objects = decoder->DecodeArrayOfObjectsForKey("NS.objects");
  std::set<NSObject*> item(make_move_iterator(objects.begin()), make_move_iterator(objects.end()));
  items_ = item;
  LOG_DEBUG("%s\n", ToString().c_str());

  return true;
}

std::string NSSet::ToString() const {
  std::stringstream ss;
  ss << "[NSSet, length=" << items_.size() << ", items=[";
  int i = 0;
  int size = items_.size();
  for (NSObject* item : items_) {
    ss << (item != nullptr ? item->ToString() : "null");
    if (i < size - 1) {
      ss << ", ";
    }
    i++;
  }
  ss << "]]";
  return ss.str();
}