#include "nskeyedarchiver/nskeyedunarchiver.hpp"

#include <cstdlib>  // for free
#include <cstring>  // for strcmp

#include "common.h"

using namespace nskeyedarchiver;

static const char* NSKeyedArchiveRootObjectKey = "root";

static const char* NSKeyedArchiveNullObjectReferenceName = "$null";
static uint64_t NSKeyedArchivePlistVersion = 100000;
static uint32_t NSKeyedArchiverSystemVersion = 2000;

// static
NSObject* NSKeyedUnarchiver::UnarchiveTopLevelObjectWithData(const char* data,
                                                             size_t length) {
  NSKeyedUnarchiver unarchiver(data, length);
  NSObject* object = unarchiver.DecodeObject(NSKeyedArchiveRootObjectKey);
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
  if (top_node == nullptr || !PLIST_IS_DICT(top_node)) {
    LOG_ERROR("Unable to read archive contents. the data may be corrupt.\n");
    return;
  }

  plist_t objects_node = plist_dict_get_item(plist, "$objects");
  if (objects_node == nullptr || !PLIST_IS_ARRAY(objects_node)) {
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

NSObject* NSKeyedUnarchiver::DecodeObject(const char* key) {
  plist_t node = ObjectInCurrentDecodingContext(key);
  if (node == nullptr) {
    LOG_ERROR("No value found for key `%s`. The data may be corrupt.\n", key);
    return nullptr;
  }

  return DecodeObject(node);
}

NSObject* NSKeyedUnarchiver::DecodeObject(plist_t node) {
  NSObject* obj = nullptr;

  if (!PLIST_IS_UID(node)) {
    LOG_ERROR("Object is not a reference. The data may be corrupt.\n");
    return obj;
  }

  plist_t dereferenced_object = DereferenceObject(node);
  if (dereferenced_object == nullptr) {
    LOG_ERROR("Invalid object reference. The data may be corrupt.\n");
    return obj;
  }

  if (PLIST_IS_STRING(dereferenced_object) && plist_string_val_compare(dereferenced_object, NSKeyedArchiveNullObjectReferenceName)) {
    return obj; // It's `$null`
  }

  if (IsContainer(dereferenced_object)) {
    LOG_DEBUG("Is Container\n");

  } else {
    LOG_DEBUG("Not Container\n");
    // TODO:
  }
  // TODO:

  return obj;
}

bool NSKeyedUnarchiver::IsContainer(plist_t node) {
  if (!PLIST_IS_DICT(node)) {
    return false;
  }

  plist_t class_node = plist_dict_get_item(node, "$class");
  return PLIST_IS_UID(class_node);
}

plist_t NSKeyedUnarchiver::DereferenceObject(plist_t node) {
  uint64_t uid = 0;
  plist_get_uid_val(node, &uid);
  LOG_DEBUG("uid=%llu, objects_size_=%u\n", uid, objects_size_);
  if (0 <= uid && uid < objects_size_) {
    return plist_array_get_item(objects_, uid);
  }
  return nullptr;
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

  return plist_dict_get_item(CurrentDecodingContext()->dict, unwrapped_key.c_str());
}

DecodingContext* NSKeyedUnarchiver::CurrentDecodingContext() {
  return containers_.top();
}