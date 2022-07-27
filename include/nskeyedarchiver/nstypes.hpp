#ifndef NSKEYEDARCHIVER_NS_TYPES_H
#define NSKEYEDARCHIVER_NS_TYPES_H

#include <string>
#include <vector>

#include "nskeyedarchiver/kavalue.hpp"
#include "nskeyedarchiver/nsclass.hpp"
#include "nskeyedarchiver/nskeyedunarchiver.hpp"

namespace nskeyedarchiver {

class NSDummy {
 public:
  static KAValue Deserialize(NSKeyedUnarchiver* decoder, const NSClass& clazz);
};

class NSDictionary {
 public:
  static KAValue Deserialize(NSKeyedUnarchiver* decoder, const NSClass& clazz);
};

class NSArray {
 public:
  static KAValue Deserialize(NSKeyedUnarchiver* decoder, const NSClass& clazz);
};

}  // namespace nskeyedarchiver

#endif  // NSKEYEDARCHIVER_NS_TYPES_H
