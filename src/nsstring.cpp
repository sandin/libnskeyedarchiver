#include "nskeyedarchiver/nsstring.hpp"

#include <sstream>

#include "nskeyedarchiver/nskeyedunarchiver.hpp"
#include "common.h"

using namespace nskeyedarchiver;

bool NSString::Decode(NSKeyedUnarchiver* decoder) {
  if (!decoder->ContainsValue("NS.string")) {
    LOG_ERROR("missing NS.string.\n");
    return false;
  }

  data_ = decoder->DecodeString("NS.string");
  LOG_DEBUG("%s\n", ToString().c_str());

  return true;
}

std::string NSString::ToString() const {
  std::stringstream ss;
  ss << "[NSString, data=\"" << data_ << "\"]";
  return ss.str();
}