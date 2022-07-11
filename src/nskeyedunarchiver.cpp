#include "nskeyedarchiver/nskeyedunarchiver.hpp"

#include <cstdlib>  // for free
#include <cstring>  // for strcmp

#include "common.h"
#include "scope.h"

using namespace nskeyedarchiver;

static const char* NSKeyedArchiveRootObjectKey = "root";

static const char* NSKeyedArchiveNullObjectReferenceName = "$null";
static uint64_t NSKeyedArchivePlistVersion = 100000;
static uint32_t NSKeyedArchiverSystemVersion = 2000;

inline static std::string get_string_from_plist(plist_t node) {
  std::string result = "";
  if (PLIST_IS_STRING(node)) {
    char* str = nullptr;
    plist_get_string_val(node, &str);
    if (str != nullptr) {
      result = str;  // clone
      free(str);
    }
  }
  return result;
}

// static
NSVariant* NSKeyedUnarchiver::UnarchiveTopLevelObjectWithData(const char* data,
                                                              size_t length) {
  NSKeyedUnarchiver unarchiver(data, length);
  NSVariant* object = static_cast<NSVariant*>(
      unarchiver.DecodeObject(NSKeyedArchiveRootObjectKey));
  return object;
}

NSKeyedUnarchiver::NSKeyedUnarchiver(const char* data, size_t length) {
  plist_t plist = nullptr;
  plist_from_bin(data, length, &plist);
  if (plist == nullptr) {
    LOG_ERROR("Can not parse the binary plist.\n");
    return;
  }
#if DEBUG
  print_plist_as_xml(plist);
#endif

  if (!PLIST_IS_DICT(plist)) {
    LOG_ERROR("Unable to read archive. the data may be corrupt.\n");
    return;
  }

  plist_t archiver_node = plist_dict_get_item(plist, "$archiver");
  if (archiver_node == nullptr ||
      plist_string_val_compare(archiver_node, "NSKeyedArchiver") != 0) {
    LOG_ERROR("Unknown archiver. the data may be corrupt.\n");
    return;
  }

  plist_t version_node = plist_dict_get_item(plist, "$version");
  if (version_node == nullptr ||
      plist_uint_val_compare(version_node, NSKeyedArchivePlistVersion) != 0) {
    LOG_ERROR("Unknown archiver version. the data may be corrupt.\n");
    return;
  }

  plist_t top_node = plist_dict_get_item(plist, "$top");
  if (!PLIST_IS_DICT(top_node)) {
    LOG_ERROR("Unable to read archive contents. the data may be corrupt.\n");
    return;
  }

  plist_t objects_node = plist_dict_get_item(plist, "$objects");
  if (!PLIST_IS_ARRAY(objects_node)) {
    LOG_ERROR("Unable to read archive contents. the data may be corrupt.\n");
    return;
  }

  plist_ = plist;
  objects_ = objects_node;
  objects_size_ = plist_array_get_size(objects_node);
  containers_.push(new DecodingContext(top_node));
}

NSKeyedUnarchiver::~NSKeyedUnarchiver() {
  if (plist_ != nullptr) {
    plist_free(plist_);
    plist_ = nullptr;
  }
}

NSCoding* NSKeyedUnarchiver::DecodeObject(const char* key) {
  plist_t node = ObjectInCurrentDecodingContext(key);
  if (node == nullptr) {
    LOG_ERROR("No value found for key `%s`. The data may be corrupt.\n", key);
    return nullptr;
  }

  return DecodeObject(node);
}

NSCoding* NSKeyedUnarchiver::DecodeObject(plist_t object_ref) {
  NSCoding* object = nullptr;

  if (!PLIST_IS_UID(object_ref)) {
    LOG_ERROR("Object is not a reference. The data may be corrupt.\n");
    return object;
  }

  plist_t dereferenced_object = DereferenceObject(object_ref);
  if (dereferenced_object == nullptr) {
    LOG_ERROR("Invalid object reference {TODO}. The data may be corrupt.\n");
    return object;
  }

  if (PLIST_IS_STRING(dereferenced_object) &&
      plist_string_val_compare(dereferenced_object,
                               NSKeyedArchiveNullObjectReferenceName) == 0) {
    return object;  // It's `$null`
  }

  if (IsContainer(dereferenced_object)) {
    LOG_DEBUG("Is Container\n");
    object = CachedObjectForReference(object_ref);
    if (object == nullptr) {
      plist_t class_reference =
          plist_dict_get_item(dereferenced_object, "$class");
      if (!PLIST_IS_UID(class_reference)) {
        LOG_ERROR("Invalid class reference {TODO}. The data may be corrupt.\n");
        return nullptr;
      }

      AnyClass* class_to_construct =
          ValidateAndMapClassReference(class_reference, allowed_classes_);
      if (class_to_construct == nullptr) {
        LOG_ERROR("Invalid class reference {TODO}. The data may be corrupt.\n");
        return nullptr;
      }

      DecodingContext* inner_decoding_context = new DecodingContext(
          dereferenced_object);  // dereferenced_object is a dict
      PushDecodingContext(inner_decoding_context);
      auto guard = make_scope_exit([&]() { PopDecodingContext(); });

      // TODO:
      // CacheObject(object_ref, object);
    }
  } else {
    LOG_DEBUG("Not Container\n");
    object = DecodePrimitive(dereferenced_object);
    if (object == nullptr) {
      LOG_ERROR("Can not decode primitive type object.\n");
    }
    // TODO:
  }
  // TODO:

  return object;
}

NSVariant* NSKeyedUnarchiver::DecodePrimitive(plist_t dereferenced_object) {
  plist_type type = plist_get_node_type(dereferenced_object);
  switch (type) {
    case PLIST_BOOLEAN:
      uint8_t b;
      plist_get_bool_val(dereferenced_object, &b);
      return new NSVariant(static_cast<bool>(b));
    case PLIST_UINT:
      uint64_t u;
      plist_get_uint_val(dereferenced_object, &u);
      return new NSVariant(u);
    case PLIST_REAL:
      double d;
      plist_get_real_val(dereferenced_object, &d);
      return new NSVariant(d);
    case PLIST_STRING:
      char* c;
      plist_get_string_val(dereferenced_object, &c);
      return new NSVariant(c);
    case PLIST_ARRAY:
    case PLIST_DICT:
    case PLIST_DATE:
    case PLIST_DATA:
    case PLIST_KEY:
    case PLIST_UID:
    case PLIST_NONE:
    default:
      LOG_ERROR("unsupport plist type %d.\n", type);
      return nullptr;
  }
}

bool NSKeyedUnarchiver::IsContainer(plist_t node) {
  if (!PLIST_IS_DICT(node)) {
    return false;
  }

  plist_t class_node = plist_dict_get_item(node, "$class");
  return PLIST_IS_UID(class_node);
}

plist_t NSKeyedUnarchiver::DereferenceObject(plist_t object_ref) {
  uint64_t uid = 0;
  plist_get_uid_val(object_ref, &uid);
  LOG_DEBUG("uid=%llu, objects_size_=%u\n", uid, objects_size_);
  if (0 <= uid && uid < objects_size_) {
    return plist_array_get_item(objects_, uid);
  }
  return nullptr;
}

NSCoding* NSKeyedUnarchiver::CachedObjectForReference(plist_t object_ref) {
  uint64_t uid = 0;
  plist_get_uid_val(object_ref, &uid);

  auto found = object_ref_map_.find(uid);
  if (found != object_ref_map_.end()) {
    return found->second;
  }
  return nullptr;
}

AnyClass* NSKeyedUnarchiver::ValidateAndMapClassReference(
    plist_t class_ref, NSKeyedUnarchiver::ClassMap allowed_classes) {
  uint64_t uid = 0;
  plist_get_uid_val(class_ref, &uid);

  auto found = classes_.find(uid);
  if (found != classes_.end()) {
    return found->second;
  }

  plist_t class_dict = DereferenceObject(class_ref);
  if (!PLIST_IS_DICT(class_dict)) {
    return nullptr;
  }

  AnyClass* class_to_construct = nullptr;
  if (!ValidateAndMapClassDictionary(class_dict, allowed_classes,
                                     &class_to_construct)) {
    LOG_ERROR("Invalid class {TODO}. The data may be corrupt.\n");
    return nullptr;
  }
  classes_[uid] = class_to_construct;
  return class_to_construct;
}

bool NSKeyedUnarchiver::ValidateAndMapClassDictionary(
    plist_t class_dict, NSKeyedUnarchiver::ClassMap allowed_classes,
    AnyClass** class_to_construct) {
  *class_to_construct = nullptr;

  plist_t classname_node = plist_dict_get_item(class_dict, "$classname");
  plist_t classhints_node = plist_dict_get_item(class_dict, "$classhints");
  plist_t classes_node = plist_dict_get_item(class_dict, "$classes");

  std::string asserted_classname = get_string_from_plist(classname_node);
  if (!asserted_classname.empty()) {
    AnyClass* asserted_class = ClassForClassName(asserted_classname);
    if (IsClassAllowed(asserted_class, allowed_classes)) {
      *class_to_construct = asserted_class;
      return true;
    }
  }

  if (PLIST_IS_ARRAY(classhints_node)) {
    plist_array_iter it;
    plist_array_new_iter(classhints_node, &it);
    plist_t item_node;
    do {
      item_node = nullptr;
      plist_array_next_item(classhints_node, it, &item_node);
      std::string asserted_classname = get_string_from_plist(item_node);
      if (!asserted_classname.empty()) {
        AnyClass* asserted_class = ClassForClassName(asserted_classname);
        if (IsClassAllowed(asserted_class, allowed_classes)) {
          *class_to_construct = asserted_class;
          free(it);
          return true;
        }
      }
    } while (item_node);
    free(it);
  }

  // TODO: Do we need to impl NSKeyedUnarchiverDelegate?

  return false;
}

AnyClass* NSKeyedUnarchiver::ClassForClassName(std::string classname) {
  return nullptr;  // TODO
}

bool NSKeyedUnarchiver::IsClassAllowed(
    AnyClass* asserted_class, NSKeyedUnarchiver::ClassMap allowed_classes) {
  if (asserted_class == nullptr) {
    return false;
  }

  // TODO: We don't support SecureCoding yet, so all classes can be allowed to
  // be decoded.

  return true;
}

std::string NSKeyedUnarchiver::EscapeArchiverKey(std::string key) {
  if (key.size() > 0 && key.at(0) == '$') {
    return "$" + key;
  } else {
    return key;
  }
}

std::string NSKeyedUnarchiver::NextGenericKey() {
  DecodingContext* ctx = CurrentDecodingContext();
  std::string key = "$" + std::to_string(ctx->generic_key);
  ctx->generic_key = ctx->generic_key + 1;
  return key;
}

plist_t NSKeyedUnarchiver::ObjectInCurrentDecodingContext(std::string key) {
  std::string unwrapped_key;

  if (!key.empty()) {
    unwrapped_key = EscapeArchiverKey(key);
  } else {
    unwrapped_key = NextGenericKey();
  }
  printf("key=%s, unwrapped_key=%s\n", key.c_str(), unwrapped_key.c_str());

  DecodingContext* ctx = CurrentDecodingContext();
  if (ctx != nullptr) {
    return plist_dict_get_item(ctx->dict, unwrapped_key.c_str());
  } else {
    return nullptr;
  }
}

DecodingContext* NSKeyedUnarchiver::CurrentDecodingContext() {
  if (!containers_.empty()) {
    return containers_.top();
  }
  return nullptr;
}

void NSKeyedUnarchiver::PushDecodingContext(DecodingContext* decoding_context) {
  containers_.push(decoding_context);
}

void NSKeyedUnarchiver::PopDecodingContext() {
  DecodingContext* ctx = containers_.top();
  containers_.pop();
  delete ctx;
}
