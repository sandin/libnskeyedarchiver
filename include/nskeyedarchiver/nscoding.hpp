#ifndef NSKEYEDARCHIVER_NSCODING_H
#define NSKEYEDARCHIVER_NSCODING_H

#include <sys/types.h>

#include "nskeyedarchiver/nsclass.hpp"

namespace nskeyedarchiver {

class NSKeyedUnarchiver;
class NSCoding {
 public:
  NSCoding() {}
  virtual ~NSCoding() {}

  virtual bool Decode(NSKeyedUnarchiver* decoder) = 0;
  // TODO: bool Encode(NSKeyedArchiver* coder) = 0;
};

}  // namespace nskeyedarchiver

#endif  // NSKEYEDARCHIVER_NSCODING_H