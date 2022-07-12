#ifndef NSKEYEDARCHIVER_NSOBJECT_H
#define NSKEYEDARCHIVER_NSOBJECT_H

#include <sys/types.h>
#include <string>

#include "nskeyedarchiver/nscoding.hpp"

namespace nskeyedarchiver {

class NSObject : public NSCoding {
 public:
  NSObject() {}
  virtual ~NSObject() {}

  virtual std::string ToString() const = 0;

  NSClass* GetClass() const { return class_; }
  void SetClass(NSClass* clazz) { class_ = clazz; }

 protected:
  NSClass* class_;
};

}  // namespace nskeyedarchiver

#endif  // NSKEYEDARCHIVER_NSOBJECT_H