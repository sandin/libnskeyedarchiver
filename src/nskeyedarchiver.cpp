#include "nskeyedarchiver/nskeyedarchiver.hpp"

#include "nskeyedarchiver/common.hpp"
#include "nskeyedarchiver/scope.hpp"

using namespace nskeyedarchiver;

NSKeyedArchiver::NSKeyedArchiver(NSClassManager* class_manager)
    : class_manager_(class_manager), output_format_(NSKeyedArchiver::Xml) {
  null_object_ = plist_new_string(kNSKeyedArchiveNullObjectReferenceName);
  objects_.emplace_back(null_object_);
  containers_.push(EncodingContext());
}

NSKeyedArchiver::~NSKeyedArchiver() {
  // free each object(`plist_t`) of the array, include the `null_object_·;
  // for (plist_t object : objects_) {
  //  plist_free(object);
  //}
  plist_free(plist_);
}

// static
bool NSKeyedArchiver::ArchivedData(const KAValue& object, char** data, size_t* size,
                                   NSKeyedArchiver::OutputFormat output_format) {
  NSClassManager& class_manager = NSClassManager::GetInstance();
  NSKeyedArchiver archiver(&class_manager);
  archiver.SetOutputFormat(output_format);
  archiver.EncodeObject(object, kNSKeyedArchiveRootObjectKey);
  if (!archiver.HasError()) {  // OK
    archiver.GetEncodedData(data, size);
    return true;
  }
  return false;
}

void NSKeyedArchiver::EncodeObject(const KAValue& object, const std::string& key) {
  ObjectRef object_ref = EncodeObject(object);
  if (object_ref != -1) {
    SetObjectInCurrentEncodingContext(object_ref, key, !key.empty());
  } else {
    LOG_ERROR("can not encode object for key: %s.\n", key.c_str());
  }
}

NSKeyedArchiver::ObjectRef NSKeyedArchiver::EncodeObject(const KAValue& object) {
  ObjectRef object_ref = ReferenceObject(object);

  if (!HaveVisited(object)) {
    plist_t encoding_object = nullptr;
    if (IsContainer(object)) {
      LOG_DEBUG("Is Container\n");

      const NSClass clazz = GetNSClass(object);
      NSClassManager::Serializer& serializer = FindClassSerializer(clazz);

      PushEncodingContext(EncodingContext());
      plist_t encoding_object = serializer(this, clazz);

      // TODO
    } else {
      LOG_DEBUG("Is Primitive\n");
      // TODO
      encoding_object = EncodePrimitive(object);
    }

    SetObject(encoding_object, object_ref);
  }
  return object_ref;
}

plist_t NSKeyedArchiver::EncodePrimitive(const KAValue& object) {
  //   enum DataType { Null, Bool, Integer, Double, Str, Raw, Object };
  switch (object.GetDataType()) {
    case KAValue::Null:
      return null_object_;
    case KAValue::Bool:
      return plist_new_bool(object.ToBool());
    case KAValue::Integer:
      return plist_new_uint(object.ToInteger());
    case KAValue::Double:
      return plist_new_real(object.ToDouble());
    case KAValue::Str:
      return plist_new_string(object.ToStr());  // copy char*
    case KAValue::Raw: {
      const KAValue::RawData* raw = object.ToRaw();
      return plist_new_data(raw->data, raw->size);  // copy char*
    }
    case KAValue::Object:  // fallthrough
    default:
      ASSERT(false, "unsupport encode KAValue type as primitive: %d.\n", object.GetDataType());
      return nullptr;
  }
}

NSClassManager::Serializer& NSKeyedArchiver::FindClassSerializer(const NSClass& clazz) const {
  if (!clazz.class_name.empty()) {
    if (class_manager_->HasSerializer(clazz.class_name)) {
      return class_manager_->GetSerializer(clazz.class_name);
    }
  }
  LOG_ERROR("Can not find the Serializer for class name: `%s`.\n", clazz.class_name.c_str());
  return class_manager_->GetDefaultSerializer();
}

NSClass NSKeyedArchiver::GetNSClass(const KAValue& object) const {
  if (object.IsObject()) {
    const KAObject inner_obj = object.ToObject<KAObject>();
    return NSClass{inner_obj.ClassName(), inner_obj.Classes(), {}};
  }
}

bool NSKeyedArchiver::IsContainer(const KAValue& object) const { return object.IsObject(); }

NSKeyedArchiver::ObjectRef NSKeyedArchiver::ReferenceObject(const KAValue& object) {
  if (object.IsNull()) {
    return kNSKeyedArchiverNullObjectReferenceUid;
  }

  NSKeyedArchiverUID uid = -1;
  auto found = obj_ref_map_.find(object);
  if (found != obj_ref_map_.end()) {
    uid = found->second;
  } else {
    uid = objects_.size();
    obj_ref_map_[object] = uid;
    objects_.emplace_back(nullptr);  // nullptr as a placeholder at position [uid]
  }

  return uid;  // TODO:
}

void NSKeyedArchiver::SetObject(plist_t encoding_object, NSKeyedArchiverUID reference) {
  objects_[reference] = encoding_object;
}

bool NSKeyedArchiver::HaveVisited(const KAValue& object) {
  if (object.IsNull()) {
    return false;  // always have a null reference
  } else {
    return obj_ref_map_.find(object) != obj_ref_map_.end();
  }
}

EncodingContext& NSKeyedArchiver::CurrentEncodingContext() {
  ASSERT(!containers_.empty(), "the containers_ can not be empty.\n");
  return containers_.top();
}

int NSKeyedArchiver::CurrentEncodingContextDepth() const { return containers_.size(); }

void NSKeyedArchiver::PushEncodingContext(EncodingContext&& decoding_context) {
  containers_.push(std::forward<EncodingContext>(decoding_context));
}

void NSKeyedArchiver::PopEncodingContext() { containers_.pop(); }

void NSKeyedArchiver::SetObjectInCurrentEncodingContext(ObjectRef object_ref,
                                                        const std::string& key, bool escape) {
  std::string encoding_key;
  if (!key.empty()) {
    if (escape) {
      encoding_key = EscapeArchiverKey(key);
    } else {
      encoding_key = key;
    }
  } else {
    encoding_key = NextGenericKey();
  }

  EncodingContext& ctx = CurrentEncodingContext();
  ctx.dict[encoding_key] = object_ref;
}

// static
std::string NSKeyedArchiver::EscapeArchiverKey(const std::string& key) {
  if (key.size() > 0 && key.at(0) == '$') {
    return "$" + key;
  } else {
    return key;
  }
}

std::string NSKeyedArchiver::NextGenericKey() {
  EncodingContext& ctx = CurrentEncodingContext();
  std::string key = "$" + std::to_string(ctx.generic_key);
  ctx.generic_key = ctx.generic_key + 1;
  return key;
}

void NSKeyedArchiver::GetEncodedData(char** data, size_t* size) {
  if (plist_ == nullptr) {
    plist_ = FinishEncoding();
  }

  if (output_format_ == OutputFormat::Xml) {
    plist_to_xml(plist_, data, (uint32_t*)size);
  } else if (output_format_ == OutputFormat::Binary) {
    plist_to_bin(plist_, data, (uint32_t*)size);
  } else {
    ASSERT(false, "unsupport output format: %d.\n", output_format_);
  }
}

plist_t NSKeyedArchiver::FinishEncoding() {
  plist_t plist = plist_new_dict();

  plist_dict_set_item(plist, "$archiver", plist_new_string(kNSKeyedArchiveName));
  plist_dict_set_item(plist, "$version", plist_new_uint(kNSKeyedArchivePlistVersion));

  plist_t objects = plist_new_array();
  for (plist_t object : objects_) {
    plist_array_append_item(objects, object);
  }
  plist_dict_set_item(plist, "$objects", objects);

  plist_t top = plist_new_dict();
  EncodingContext& ctx = CurrentEncodingContext();
  for (auto kv : ctx.dict) {
    plist_t uid = plist_new_uid(kv.second);
    plist_dict_set_item(top, kv.first.c_str(), uid);
  }
  plist_dict_set_item(plist, "$top", top);

  return plist;
}
