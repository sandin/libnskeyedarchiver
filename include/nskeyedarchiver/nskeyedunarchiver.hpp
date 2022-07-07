#ifndef NSKEYEDARCHIVER_NSKEYEDUNARCHIVER_H
#define NSKEYEDARCHIVER_NSKEYEDUNARCHIVER_H

#include <sys/types.h>

#include <stack>
#include <string>

#include "nskeyedarchiver/nsobject.hpp"
#include "plist/plist.h"

namespace nskeyedarchiver {

struct DecodingContext {
  plist_t dict;  // parent node, plist type: dict
  uint64_t generic_key;

  DecodingContext(plist_t in_dict) : dict(in_dict), generic_key(0) {} 
};

class NSKeyedUnarchiver {
 public:
  NSKeyedUnarchiver(const char* data, size_t length);
  virtual ~NSKeyedUnarchiver();

  NSObject* DecodeObject(const char* key);

  static NSObject* UnarchiveTopLevelObjectWithData(const char* data,
                                                   size_t length);

 private:
  NSObject* DecodeObject(plist_t node);
  plist_t DereferenceObject(plist_t node);

  DecodingContext* CurrentDecodingContext();
  std::string NextGenericKey();
  std::string EscapeArchiverKey(std::string key);
  plist_t ObjectInCurrentDecodingContext(std::string key);
  bool IsContainer(plist_t node);

  plist_t plist_ = nullptr;    // root node, plist type: dict
  plist_t objects_ = nullptr;  // `$objects` node, plist type: array
  uint32_t objects_size_ = 0;  // size of `$objects` array
  std::stack<DecodingContext*> containers_;

};  // class NSKeyedUnarchiver

}  // namespace nskeyedarchiver

#endif  // NSKEYEDARCHIVER_NSKEYEDUNARCHIVER_H