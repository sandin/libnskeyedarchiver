#ifndef NSKEYEDARCHIVER_NSKEYEDARCHIVER_H
#define NSKEYEDARCHIVER_NSKEYEDARCHIVER_H

#include <sys/types.h>

#include <functional>
#include <map>
#include <stack>
#include <string>
#include <vector>

#include "nskeyedarchiver/kaarray.hpp"
#include "nskeyedarchiver/kaobject.hpp"
#include "nskeyedarchiver/kavalue.hpp"
#include "nskeyedarchiver/nsclass.hpp"
#include "nskeyedarchiver/nsclassmanager.hpp"
#include "plist/plist.h"

namespace nskeyedarchiver {

static const char kNSKeyedArchiveName[] = "NSKeyedArchiver";
static const char kNSKeyedArchiveRootObjectKey[] = "root";
static const char kNSKeyedArchiveNullObjectReferenceName[] = "$null";
static const uint64_t kNSKeyedArchivePlistVersion = 100000;
static const uint32_t kNSKeyedArchiverSystemVersion = 2000;
static const uint32_t kNSKeyedArchiverNullObjectReferenceUid = 0;
static const int32_t kInvalidNSKeyedArchiverUID = -1;

struct EncodingContext {
  plist_t dict;  // the object container that is being encoded, type: PLIST_DICT
  uint64_t generic_key;

  EncodingContext() : generic_key(0), dict(plist_new_dict()) {}
  ~EncodingContext() {
    // NOTE: DO NOT FREE THE `dict`.
    // Although it was created by `EncodingContext`, but after encoding,
    // it will be placed in the `plist_t_` and its ownership will be transferred to
    // `NSKeyedArchiver` `NSKeyedArchiver` is responsible for freeing it
  }
};

class NSKeyedArchiverUID {
 public:
  NSKeyedArchiverUID() : value_(kInvalidNSKeyedArchiverUID) {}
  NSKeyedArchiverUID(int64_t value) : value_(value) {}

  ~NSKeyedArchiverUID() {}

  bool IsValid() const { return value_ != kInvalidNSKeyedArchiverUID; }

  uint64_t Value() const { return static_cast<uint64_t>(value_); }

  plist_t AsRef() {
    return plist_new_uid(value_);  // delayed initialization
  }

 private:
  plist_t uid_plist_ = nullptr;
  int64_t value_;
};  // class NSKeyedArchiverUID

template <typename ValueType>
class KAValueUIDMap {
 public:
  using KAValueRef = std::reference_wrapper<const KAValue>;

  void Put(const KAValue& key, const ValueType& value) {
    int bucket_id = static_cast<int>(key.GetDataType());
    buckets_[bucket_id].emplace_back(std::make_pair(KAValueRef(key), value));  // ref
  }

  const ValueType& Get(const KAValue& key, const ValueType& def_val) {
    int bucket_id = static_cast<int>(key.GetDataType());
    for (const auto& it : buckets_[bucket_id]) {
      if (key.Equals(it.first)) {
        return it.second;
      }
    }
    return def_val;
  }

  bool Contains(const KAValue& key) {
    int bucket_id = static_cast<int>(key.GetDataType());
    for (const auto& it : buckets_[bucket_id]) {
      if (key.Equals(it.first)) {
        return true;
      }
    }
    return false;
  }

 private:
  std::vector<std::pair<KAValueRef, ValueType>> buckets_[KAValue::DataType::Count];
};

class NSKeyedArchiver {
 public:
  using ObjectRef = plist_t;                               // TYPE: PLIST_UID
  using ObjectUidMap = KAValueUIDMap<NSKeyedArchiverUID>;  // std::map<KAValue, NSKeyedArchiverUID,
                                                           // // KAValueComparator>; // TODO: can we
                                                           // avoid copying KAValue here?
  using ClassUidMap = std::map<std::string, NSKeyedArchiverUID>;
  using ObjectArray = const std::vector<KAValue>&;
  using ObjectRefArray = const std::vector<std::reference_wrapper<const KAValue>>&;

  enum OutputFormat { Xml, Binary };

  NSKeyedArchiver(NSClassManager* class_manager);
  virtual ~NSKeyedArchiver();

  void EncodeObject(const KAValue& object, const std::string& key);
  void EncodePrimitive(const KAValue& object, const std::string& key);
  void EncodeArrayOfObjects(ObjectArray array, const std::string& key);
  void EncodeArrayOfObjectRefs(ObjectRefArray array, const std::string& key);

  // Caller is responsible for freeing the output data.
  void GetEncodedData(char** data, size_t* size);

  static bool ArchivedData(
      const KAValue& object, char** data, size_t* size,
      NSKeyedArchiver::OutputFormat output_format = NSKeyedArchiver::OutputFormat::Binary);

  static std::string EscapeArchiverKey(const std::string& key);

 private:
  ObjectRef EncodeObject(const KAValue& object);
  NSKeyedArchiverUID ReferenceObject(const KAValue& object);
  NSKeyedArchiverUID GetReferencedObjectUid(const KAValue& object);
  NSKeyedArchiverUID ReferenceClass(const NSClass& clazz);

  plist_t EncodeClass(const NSClass& clazz);
  plist_t EncodePrimitive(const KAValue& object);
  void EncodeValue(ObjectRef plist, const std::string& key);
  plist_t FinishEncoding();

  void SetObject(NSKeyedArchiverUID uid, plist_t encoding_object);

  EncodingContext& CurrentEncodingContext();
  int CurrentEncodingContextDepth() const;
  void SetObjectInCurrentEncodingContext(ObjectRef object_ref, const std::string& key,
                                         bool escape = true);
  void PushEncodingContext(EncodingContext&& decoding_context);
  void PopEncodingContext();

  bool IsContainer(const KAValue& object) const;
  NSClass GetNSClass(const KAValue& object) const;

  NSClassManager::Serializer& FindClassSerializer(const NSClass& clazz) const;

  std::string NextGenericKey();

  bool HasError() const { return !error_.empty(); }
  const std::string& Error() const { return error_; }

  void SetOutputFormat(OutputFormat output_format) { output_format_ = output_format; }

  OutputFormat output_format_;
  plist_t plist_ = nullptr;
  plist_t null_object_ = nullptr;
  ObjectUidMap obj_uid_map_;  // record the uid of each Object, same Object(equals) has
                              // the same uid
  ClassUidMap class_uid_map_;
  std::string error_;
  std::vector<plist_t> objects_;  // the `$objects` array, the index of the element is the
                                  // reference(uid) of the object
  std::stack<EncodingContext> containers_;
  NSClassManager* class_manager_;

};  // class NSKeyedArchiver

}  // namespace nskeyedarchiver

#endif  // NSKEYEDARCHIVER_NSKEYEDARCHIVER_H
