#ifndef NSKEYEDARCHIVER_NSKEYEDUNARCHIVER_H
#define NSKEYEDARCHIVER_NSKEYEDUNARCHIVER_H

#include <sys/types.h>

#include <stack>
#include <string>
#include <unordered_map>

#include "nskeyedarchiver/nscoding.hpp"
#include "nskeyedarchiver/nsvariant.hpp"
#include "plist/plist.h"

namespace nskeyedarchiver {

struct DecodingContext {
  plist_t dict;  // parent node, plist type: dict
  uint64_t generic_key;

  DecodingContext(plist_t in_dict) : dict(in_dict), generic_key(0) {}
};

struct AnyClass {
  std::string class_name;
};

class NSKeyedUnarchiver {
 public:
  using ClassMap = std::unordered_map<uint64_t, AnyClass*>;
  using ObjectMap = std::unordered_map<uint64_t, NSCoding*>;

  NSKeyedUnarchiver(const char* data, size_t length);
  virtual ~NSKeyedUnarchiver();

  NSCoding* DecodeObject(const char* key);

  static NSVariant* UnarchiveTopLevelObjectWithData(const char* data,
                                                    size_t length);

 private:
  NSVariant* DecodePrimitive(plist_t dereferenced_object);
  NSCoding* DecodeObject(plist_t object_ref);
  plist_t DereferenceObject(plist_t object_ref);
  NSCoding* CachedObjectForReference(plist_t object_ref);

  DecodingContext* CurrentDecodingContext();
  plist_t ObjectInCurrentDecodingContext(std::string key);
  void PushDecodingContext(DecodingContext* decoding_context);
  void PopDecodingContext();

  std::string NextGenericKey();
  std::string EscapeArchiverKey(std::string key);

  bool IsContainer(plist_t node);

  AnyClass* ValidateAndMapClassReference(plist_t class_ref,
                                         ClassMap allowed_classes);
  bool ValidateAndMapClassDictionary(plist_t class_dict,
                                     ClassMap allowed_classes,
                                     AnyClass** class_to_construct);
  bool IsClassAllowed(AnyClass* asserted_class, ClassMap allowed_classes);
  AnyClass* ClassForClassName(std::string classname);

  plist_t plist_ = nullptr;    // root node, plist type: dict
  plist_t objects_ = nullptr;  // `$objects` node, plist type: array
  uint32_t objects_size_ = 0;  // size of `$objects` array
  std::stack<DecodingContext*> containers_;
  ObjectMap object_ref_map_;
  ClassMap allowed_classes_;
  ClassMap classes_;

};  // class NSKeyedUnarchiver

}  // namespace nskeyedarchiver

#endif  // NSKEYEDARCHIVER_NSKEYEDUNARCHIVER_H