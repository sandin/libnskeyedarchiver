#ifndef NSKEYEDARCHIVER_NSKEYEDUNARCHIVER_H
#define NSKEYEDARCHIVER_NSKEYEDUNARCHIVER_H

#include "plist/plist.h"
#include <sys/types.h>

#include "nskeyedarchiver/nsobject.h"

namespace nskeyedarchiver {

class NSKeyedUnarchiver {
 public:
  NSKeyedUnarchiver() {}
  ~NSKeyedUnarchiver() {}

  NSObject* Unarchive(const char* data, size_t length);
};

}  // namespace nskeyedarchiver

#endif  // NSKEYEDARCHIVER_NSKEYEDUNARCHIVER_H