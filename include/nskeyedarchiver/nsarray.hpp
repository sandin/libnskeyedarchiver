#ifndef NSKEYEDARCHIVER_NSARRAY_H
#define NSKEYEDARCHIVER_NSARRAY_H

#include <sys/types.h>

#include <vector>

#include "nskeyedarchiver/nsclass.hpp"
#include "nskeyedarchiver/nsobject.hpp"

namespace nskeyedarchiver {

class NSArray : public NSObject {
 public:
  virtual bool Decode(NSKeyedUnarchiver* decoder);

  virtual std::string ToString() const;

 private:
  std::vector<NSObject*> items_;
};
DECLARE_NSCLASS(NSArray);

}  // namespace nskeyedarchiver

#endif  // NSKEYEDARCHIVER_NSARRAY_H