#ifndef NSKEYEDARCHIVER_NSDICTIONARY_H
#define NSKEYEDARCHIVER_NSDICTIONARY_H

#include <sys/types.h>

#include <string>
#include <unordered_map>

#include "nskeyedarchiver/nsclass.hpp"
#include "nskeyedarchiver/nsobject.hpp"

namespace nskeyedarchiver {

class NSDictionary : public NSObject {
 public:
  virtual bool Decode(NSKeyedUnarchiver* decoder);

  virtual std::string ToString() const;

 private:
  std::unordered_map<NSObject*, NSObject*> items_;
};
DECLARE_NSCLASS(NSDictionary);

class NSMutableDictionary : public NSDictionary {};
DECLARE_NSCLASS(NSMutableDictionary);

}  // namespace nskeyedarchiver

#endif  // NSKEYEDARCHIVER_NSDICTIONARY_H