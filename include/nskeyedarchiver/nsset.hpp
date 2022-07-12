#ifndef NSKEYEDARCHIVER_NSSET_H
#define NSKEYEDARCHIVER_NSSET_H

#include <sys/types.h>

#include <set>

#include "nskeyedarchiver/nsclass.hpp"
#include "nskeyedarchiver/nsobject.hpp"

namespace nskeyedarchiver {

class NSSet : public NSObject {
 public:
  virtual bool Decode(NSKeyedUnarchiver* decoder);

  virtual std::string ToString() const;
 private:
  std::set<NSObject*> items_;
};
DECLARE_NSCLASS(NSSet);

class NSMutableSet : public NSSet {};
DECLARE_NSCLASS(NSMutableSet);

}  // namespace nskeyedarchiver

#endif  // NSKEYEDARCHIVER_NSSET_H