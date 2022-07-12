#ifndef NSKEYEDARCHIVER_NSSTRING_H
#define NSKEYEDARCHIVER_NSSTRING_H

#include <sys/types.h>

#include <set>

#include "nskeyedarchiver/nsclass.hpp"
#include "nskeyedarchiver/nsobject.hpp"

namespace nskeyedarchiver {

class NSString : public NSObject {
 public:
  virtual bool Decode(NSKeyedUnarchiver* decoder);

  virtual std::string ToString() const;
 private:
  std::string data_;
};
DECLARE_NSCLASS(NSString);

class NSMutableString : public NSString {};
DECLARE_NSCLASS(NSMutableString);

}  // namespace nskeyedarchiver

#endif  // NSKEYEDARCHIVER_NSSTRING_H