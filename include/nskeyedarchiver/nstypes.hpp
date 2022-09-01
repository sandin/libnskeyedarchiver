#ifndef NSKEYEDARCHIVER_NS_TYPES_H
#define NSKEYEDARCHIVER_NS_TYPES_H

#include <string>
#include <vector>

#include "nskeyedarchiver/kavalue.hpp"
#include "nskeyedarchiver/nsclass.hpp"
#include "nskeyedarchiver/nskeyedarchiver.hpp"
#include "nskeyedarchiver/nskeyedunarchiver.hpp"
#include "plist/plist.h"

namespace nskeyedarchiver {

class NSDummy {
 public:
  static bool Serialize(NSKeyedArchiver* encoder, const NSClass& clazz, const KAValue& object);
  static KAValue Deserialize(NSKeyedUnarchiver* decoder, const NSClass& clazz);
};

class NSDictionary {
 public:
  static bool Serialize(NSKeyedArchiver* encoder, const NSClass& clazz, const KAValue& object);
  static KAValue Deserialize(NSKeyedUnarchiver* decoder, const NSClass& clazz);
};

class NSArray {
 public:
  static bool Serialize(NSKeyedArchiver* encoder, const NSClass& clazz, const KAValue& object);
  static KAValue Deserialize(NSKeyedUnarchiver* decoder, const NSClass& clazz);
};

class NSString {
 public:
  static bool Serialize(NSKeyedArchiver* encoder, const NSClass& clazz, const KAValue& object);
  static KAValue Deserialize(NSKeyedUnarchiver* decoder, const NSClass& clazz);
};

class NSNull {
 public:
  static bool Serialize(NSKeyedArchiver* encoder, const NSClass& clazz, const KAValue& object);
  static KAValue Deserialize(NSKeyedUnarchiver* decoder, const NSClass& clazz);
};

class NSDate {
 public:
  static bool Serialize(NSKeyedArchiver* encoder, const NSClass& clazz, const KAValue& object);
  static KAValue Deserialize(NSKeyedUnarchiver* decoder, const NSClass& clazz);
};

class NSError {
 public:
  static bool Serialize(NSKeyedArchiver* encoder, const NSClass& clazz, const KAValue& object);
  static KAValue Deserialize(NSKeyedUnarchiver* decoder, const NSClass& clazz);
};

class DTSysmonTapMessage {
 public:
  static bool Serialize(NSKeyedArchiver* encoder, const NSClass& clazz, const KAValue& object);
  static KAValue Deserialize(NSKeyedUnarchiver* decoder, const NSClass& clazz);
};

}  // namespace nskeyedarchiver

#endif  // NSKEYEDARCHIVER_NS_TYPES_H
