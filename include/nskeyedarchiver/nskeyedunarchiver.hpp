#ifndef NSKEYEDARCHIVER_NSKEYEDUNARCHIVER_H
#define NSKEYEDARCHIVER_NSKEYEDUNARCHIVER_H

#include <sys/types.h>

#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

#include "nskeyedarchiver/kaobject.hpp"
#include "nskeyedarchiver/kavalue.hpp"
#include "nskeyedarchiver/nsclass.hpp"
#include "nskeyedarchiver/nsclassmanager.hpp"
#include "plist/plist.h"

namespace nskeyedarchiver {

struct DecodingContext {
  plist_t dict;  // parent node, plist type: dict
  uint64_t generic_key;

  DecodingContext(plist_t in_dict) : dict(in_dict), generic_key(0) {}
};

class NSKeyedUnarchiver {
 public:
  NSKeyedUnarchiver(NSClassManager* class_manager, const char* data, size_t length);
  virtual ~NSKeyedUnarchiver();

  KAValue DecodeObject(const std::string& key);
  bool ContainsValue(const std::string& key);
  std::vector<KAValue> DecodeArrayOfObjectsForKey(const std::string& key);
  std::string DecodeString(const std::string& key);
  double DecodeDouble(const std::string& key);
  uint64_t DecodeInteger(const std::string& key);

  static KAValue UnarchiveTopLevelObjectWithData(const char* data, size_t length);

 private:
  KAValue DecodePrimitive(plist_t dereferenced_object) const;
  KAValue DecodeObject(plist_t object_ref);
  plist_t DecodeValue(std::string key);
  plist_t DereferenceObject(plist_t object_ref) const;
  NSClass DereferenceClass(plist_t class_ref) const;

  DecodingContext& CurrentDecodingContext();
  int CurrentDecodingContextDepth() const;
  plist_t ObjectInCurrentDecodingContext(const std::string& key);
  void PushDecodingContext(DecodingContext&& decoding_context);
  void PopDecodingContext();

  std::string NextGenericKey();

  bool IsContainer(plist_t node) const;

  NSClassManager::Deserializer& FindClassDeserializer(const NSClass& clazz) const;

  plist_t plist_ = nullptr;    // root node, plist type: dict
  plist_t objects_ = nullptr;  // `$objects` node, plist type: array
  uint32_t objects_size_ = 0;  // size of `$objects` array
  std::stack<DecodingContext> containers_;
  NSClassManager* class_manager_;

};  // class NSKeyedUnarchiver

}  // namespace nskeyedarchiver

#endif  // NSKEYEDARCHIVER_NSKEYEDUNARCHIVER_H
