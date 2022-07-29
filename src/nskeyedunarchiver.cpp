#include "nskeyedarchiver/nskeyedunarchiver.hpp"

#include <cstdlib>  // for free
#include <cstring>  // for strcmp

#include "nskeyedarchiver/common.hpp"
#include "nskeyedarchiver/scope.hpp"

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

inline static std::vector<std::string> get_string_list_from_plist(plist_t array_node) {
  std::vector<std::string> result;
  if (PLIST_IS_ARRAY(array_node)) {
    plist_array_iter it;
    plist_array_new_iter(array_node, &it);
    plist_t item_node;
    do {
      item_node = nullptr;
      plist_array_next_item(array_node, it, &item_node);
      result.emplace_back(get_string_from_plist(item_node));
    } while (item_node);
    free(it);
  }
  return result;
}

// static
KAValue NSKeyedUnarchiver::UnarchiveTopLevelObjectWithData(const char* data, size_t length) {
  NSClassManager& class_manager = NSClassManager::GetInstance();
  NSKeyedUnarchiver unarchiver(&class_manager, data, length);
  return unarchiver.DecodeObject(NSKeyedArchiveRootObjectKey);
}

NSKeyedUnarchiver::NSKeyedUnarchiver(NSClassManager* class_factory, const char* data, size_t length)
    : class_manager_(class_factory) {
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
  if (archiver_node == nullptr || plist_string_val_compare(archiver_node, "NSKeyedArchiver") != 0) {
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
  containers_.push(DecodingContext(top_node));
}

NSKeyedUnarchiver::~NSKeyedUnarchiver() {
  if (plist_ != nullptr) {
    plist_free(plist_);
    plist_ = nullptr;
  }
}

KAValue NSKeyedUnarchiver::DecodeObject(const std::string& key) {
  if (plist_ == nullptr) {
    return KAValue(); // null
  }
  
  plist_t node = ObjectInCurrentDecodingContext(key);
  LOG_DEBUG("depth=%d, key=%s\n", CurrentDecodingContextDepth(), key.c_str());
  if (node == nullptr) {
    LOG_ERROR("No value found for key `%s`. The data may be corrupt.\n", key.c_str());
    return KAValue();  // null
  }

  return DecodeObject(node);
}

KAValue NSKeyedUnarchiver::DecodeObject(plist_t object_ref) {
  if (!PLIST_IS_UID(object_ref)) {
    LOG_ERROR("Object is not a reference. The data may be corrupt.\n");
    return KAValue();  // null
  }

  plist_t dereferenced_object = DereferenceObject(object_ref);
  if (dereferenced_object == nullptr) {
    LOG_ERROR("Invalid object reference {TODO}. The data may be corrupt.\n");
    return KAValue();  // null
  }
  //#if DEBUG
  //  print_plist_as_xml(dereferenced_object);
  //#endif

  if (PLIST_IS_STRING(dereferenced_object) &&
      plist_string_val_compare(dereferenced_object, NSKeyedArchiveNullObjectReferenceName) == 0) {
    return KAValue();  // It's `$null`
  }

  if (IsContainer(dereferenced_object)) {
    LOG_DEBUG("Is Container\n");
    plist_t class_reference = plist_dict_get_item(dereferenced_object, "$class");
    if (!PLIST_IS_UID(class_reference)) {
      LOG_ERROR("Invalid class reference {TODO}. The data may be corrupt.\n");
      return KAValue();  // null;
    }

    const NSClass clazz = DereferenceClass(class_reference);
    NSClassManager::Deserializer& deserializer = FindClassDeserializer(clazz);

    PushDecodingContext(DecodingContext(dereferenced_object));  // dereferenced_object is a dict
    auto guard = make_scope_exit([&]() { PopDecodingContext(); });

    return deserializer(this, clazz);
  } else {
    LOG_DEBUG("Is Primitive\n");
    return DecodePrimitive(dereferenced_object);
  }
}

KAValue NSKeyedUnarchiver::DecodePrimitive(plist_t dereferenced_object) const {
  plist_type type = plist_get_node_type(dereferenced_object);
  switch (type) {
    case PLIST_BOOLEAN: {
      uint8_t b;
      plist_get_bool_val(dereferenced_object, &b);
      LOG_DEBUG("val=%d\n", b);
      return KAValue(static_cast<bool>(b));
    }
    case PLIST_UINT: {
      uint64_t u;
      plist_get_uint_val(dereferenced_object, &u);
      LOG_DEBUG("val=%llu\n", u);
      return KAValue(u);
    }
    case PLIST_REAL: {
      double d;
      plist_get_real_val(dereferenced_object, &d);
      LOG_DEBUG("val=%f\n", d);
      return KAValue(d);
    }
    case PLIST_STRING: {
      char* c;
      plist_get_string_val(dereferenced_object, &c);
      LOG_DEBUG("val=%s\n", c);
      KAValue value(c);
      free(c);
      return value;
    }
    case PLIST_DATA: {
      char* d;
      uint64_t size;
      plist_get_data_val(dereferenced_object, &d, &size);
      LOG_DEBUG("val=%p, size=%llu\n", d, size);
      KAValue::RawData* raw = new KAValue::RawData;
      raw->size = size;
      raw->data = static_cast<char*>(malloc(size));
      memcpy(raw->data, d, size);
      
      KAValue value(raw);
      free(d);
      return value;
    }
    case PLIST_ARRAY:
    case PLIST_DICT:
    case PLIST_DATE:
    case PLIST_KEY:
    case PLIST_UID:
    case PLIST_NONE:
    default:
      LOG_ERROR("unsupport plist type %d.\n", type);
      return KAValue();  // null
  }
}

bool NSKeyedUnarchiver::IsContainer(plist_t node) const {
  if (!PLIST_IS_DICT(node)) {
    return false;
  }

  plist_t class_node = plist_dict_get_item(node, "$class");
  return PLIST_IS_UID(class_node);
}

plist_t NSKeyedUnarchiver::DereferenceObject(plist_t object_ref) const {
  uint64_t uid = 0;
  plist_get_uid_val(object_ref, &uid);
  LOG_DEBUG("deref obj, uid=%llu, objects_size_=%u\n", uid, objects_size_);
  if (0 <= uid && uid < objects_size_) {
    return plist_array_get_item(objects_, uid);
  }
  return nullptr;
}

NSClass NSKeyedUnarchiver::DereferenceClass(plist_t class_ref) const {
  NSClass clazz{"", {}, {}};
  plist_t class_dict = DereferenceObject(class_ref);
  if (!PLIST_IS_DICT(class_dict)) {
    return clazz;
  }

  plist_t classname_node = plist_dict_get_item(class_dict, "$classname");
  plist_t classhints_node = plist_dict_get_item(class_dict, "$classhints");
  plist_t classes_node = plist_dict_get_item(class_dict, "$classes");

  clazz.class_name = get_string_from_plist(classname_node);
  clazz.classes = get_string_list_from_plist(classes_node);
  clazz.classhints = get_string_list_from_plist(classhints_node);

  return clazz;
}

NSClassManager::Deserializer& NSKeyedUnarchiver::FindClassDeserializer(const NSClass& clazz) const {
  if (!clazz.class_name.empty()) {
    if (class_manager_->HasDeserializer(clazz.class_name)) {
      return class_manager_->GetDeserializer(clazz.class_name);
    }
  }
  for (const std::string& class_name : clazz.classhints) {
    if (class_manager_->HasDeserializer(class_name)) {
      return class_manager_->GetDeserializer(class_name);
    }
  }

  LOG_ERROR("Can not find the Deserializer for class name: `%s`.\n", clazz.class_name.c_str());
  return class_manager_->GetDefaultDeserializer();
}

std::string NSKeyedUnarchiver::EscapeArchiverKey(const std::string& key) const {
  if (key.size() > 0 && key.at(0) == '$') {
    return "$" + key;
  } else {
    return key;
  }
}

std::string NSKeyedUnarchiver::NextGenericKey() {
  DecodingContext& ctx = CurrentDecodingContext();
  std::string key = "$" + std::to_string(ctx.generic_key);
  ctx.generic_key = ctx.generic_key + 1;
  return key;
}

plist_t NSKeyedUnarchiver::ObjectInCurrentDecodingContext(const std::string& key) {
  std::string unwrapped_key;

  if (!key.empty()) {
    unwrapped_key = EscapeArchiverKey(key);
  } else {
    unwrapped_key = NextGenericKey();
  }
  // LOG_DEBUG("key=%s, unwrapped_key=%s\n", key.c_str(),
  // unwrapped_key.c_str());

  DecodingContext& ctx = CurrentDecodingContext();
  return plist_dict_get_item(ctx.dict, unwrapped_key.c_str());
}

DecodingContext& NSKeyedUnarchiver::CurrentDecodingContext() {
  ASSERT(!containers_.empty());
  return containers_.top();
}

int NSKeyedUnarchiver::CurrentDecodingContextDepth() const { return containers_.size(); }

void NSKeyedUnarchiver::PushDecodingContext(DecodingContext&& decoding_context) {
  containers_.push(std::forward<DecodingContext>(decoding_context));
}

void NSKeyedUnarchiver::PopDecodingContext() { containers_.pop(); }

plist_t NSKeyedUnarchiver::DecodeValue(std::string key) {
  return ObjectInCurrentDecodingContext(key);
}

bool NSKeyedUnarchiver::ContainsValue(const std::string& key) {
  plist_t val = DecodeValue(key);
  return val != nullptr;
}

std::vector<KAValue> NSKeyedUnarchiver::DecodeArrayOfObjectsForKey(const std::string& key) {
  std::vector<KAValue> array;
  plist_t object_refs = DecodeValue(key);

  if (PLIST_IS_ARRAY(object_refs)) {
    plist_array_iter it;
    plist_array_new_iter(object_refs, &it);
    plist_t item_node;
    do {
      item_node = nullptr;
      plist_array_next_item(object_refs, it, &item_node);

      if (PLIST_IS_UID(item_node)) {
        KAValue object = DecodeObject(item_node);
        array.emplace_back(object);
      }
    } while (item_node);
    free(it);
  }

  return array;
}

std::string NSKeyedUnarchiver::DecodeString(const std::string& key) {
  plist_t value_node = DecodeValue(key);
  if (PLIST_IS_STRING(value_node)) {
    char* c;
    plist_get_string_val(value_node, &c);
    std::string str = c;
    delete c;
    return str;
  }
  LOG_ERROR("%s is not a string, node_type=%d\n", key.c_str(), plist_get_node_type(value_node));
  return "";  // TODO: throw exception?
}
