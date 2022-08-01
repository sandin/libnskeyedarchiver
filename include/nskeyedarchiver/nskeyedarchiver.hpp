#ifndef NSKEYEDARCHIVER_NSKEYEDARCHIVER_H
#define NSKEYEDARCHIVER_NSKEYEDARCHIVER_H

#include <sys/types.h>

#include <map>
#include <stack>
#include <string>
#include <vector>

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

using NSKeyedArchiverUID = int32_t;

struct EncodingContext {
  std::map<std::string, NSKeyedArchiverUID> dict;  // the object container that is being encoded
  uint64_t generic_key;

  EncodingContext() : generic_key(0) {}
};

class NSKeyedArchiver {
 public:
  using ObjectRef = NSKeyedArchiverUID;
  using ObjectRefMap = std::map<KAValue, ObjectRef, KAValueComparator>;
  enum OutputFormat { Xml, Binary };

  NSKeyedArchiver(NSClassManager* class_manager);
  virtual ~NSKeyedArchiver();

  void EncodeObject(const KAValue& object, const std::string& key);
  // Caller is responsible for freeing the output data.
  void GetEncodedData(char** data, size_t* size);

  static bool ArchivedData(
      const KAValue& object, char** data, size_t* size,
      NSKeyedArchiver::OutputFormat output_format = NSKeyedArchiver::OutputFormat::Binary);

  static std::string EscapeArchiverKey(const std::string& key);

 private:
  ObjectRef EncodeObject(const KAValue& object);
  ObjectRef ReferenceObject(const KAValue& object);
  plist_t EncodePrimitive(const KAValue& object);
  void SetObject(plist_t encoding_object, NSKeyedArchiverUID reference);

  plist_t FinishEncoding();

  EncodingContext& CurrentEncodingContext();
  int CurrentEncodingContextDepth() const;
  void SetObjectInCurrentEncodingContext(ObjectRef object_ref, const std::string& key, bool escape);
  void PushEncodingContext(EncodingContext&& decoding_context);
  void PopEncodingContext();

  bool IsContainer(const KAValue& object) const;
  NSClass GetNSClass(const KAValue& object) const;

  NSClassManager::Serializer& FindClassSerializer(const NSClass& clazz) const;

  std::string NextGenericKey();

  bool HaveVisited(const KAValue& object);

  bool HasError() const { error_.empty(); };
  const std::string& Error() const { return error_; }

  void SetOutputFormat(OutputFormat output_format) { output_format_ = output_format; }

  OutputFormat output_format_;
  plist_t plist_ = nullptr;
  plist_t null_object_ = nullptr;
  ObjectRefMap obj_ref_map_;
  std::string error_;
  std::vector<plist_t> objects_;
  std::stack<EncodingContext> containers_;
  NSClassManager* class_manager_;

};  // class NSKeyedArchiver

}  // namespace nskeyedarchiver

#endif  // NSKEYEDARCHIVER_NSKEYEDARCHIVER_H
