#ifndef NSKEYEDARCHIVER_NSCODING_H
#define NSKEYEDARCHIVER_NSCODING_H

#include <sys/types.h>

namespace nskeyedarchiver {

class NSKeyedUnarchiver;

class NSCoding {
 public:
  NSCoding() {}
  virtual ~NSCoding() {}

  virtual bool Decode(NSKeyedUnarchiver* decoder) = 0;
  //TODO: bool encode();
};

}  // namespace nskeyedarchiver

#endif  // NSKEYEDARCHIVER_NSCODING_H