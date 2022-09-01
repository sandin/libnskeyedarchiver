#include "nskeyedarchiver/nsclassmanager.hpp"

#include "nskeyedarchiver/nstypes.hpp"

using namespace nskeyedarchiver;

NSClassManager::NSClassManager()
    : default_serializer_(NSDummy::Serialize), default_deserializer_(NSDummy::Deserialize) {
#define REGISTER_SERIALIZERS(name, cls)     \
  RegisterSerializer(name, cls::Serialize); \
  RegisterDeserializer(name, cls::Deserialize);

  REGISTER_SERIALIZERS("NSDictionary", NSDictionary);
  REGISTER_SERIALIZERS("NSMutableDictionary", NSDictionary);
  REGISTER_SERIALIZERS("NSArray", NSArray);
  REGISTER_SERIALIZERS("NSMutableArray", NSArray);
  REGISTER_SERIALIZERS("NSSet", NSArray);
  REGISTER_SERIALIZERS("NSMutableSet", NSArray);
  REGISTER_SERIALIZERS("NSString", NSString);
  REGISTER_SERIALIZERS("NSMutableString", NSString);
  RegisterDeserializer("NSDate", NSDate::Deserialize); // TODO
  RegisterDeserializer("NSError", NSError::Deserialize); // TODO
  RegisterDeserializer("NSNull", NSNull::Deserialize); // TODO

  RegisterDeserializer("DTSysmonTapMessage", DTSysmonTapMessage::Deserialize);
#undef REGISTER_SERIALIZERS
}

NSClassManager::~NSClassManager() {
  deserializer_map_.clear();
  serializer_map_.clear();
}

void NSClassManager::RegisterDeserializer(const std::string& class_name,
                                          Deserializer deserializer) {
  deserializer_map_[class_name] = deserializer;
}

void NSClassManager::UnregisterDeserializer(const std::string& class_name) {
  deserializer_map_.erase(class_name);
}

void NSClassManager::RegisterSerializer(const std::string& class_name, Serializer serializer) {
  serializer_map_[class_name] = serializer;
}

void NSClassManager::UnregisterSerializer(const std::string& class_name) {
  serializer_map_.erase(class_name);
}

bool NSClassManager::HasDeserializer(const std::string& class_name) {
  return deserializer_map_.find(class_name) != deserializer_map_.end();
}

bool NSClassManager::HasSerializer(const std::string& class_name) {
  return serializer_map_.find(class_name) != serializer_map_.end();
}

NSClassManager::Deserializer& NSClassManager::GetDeserializer(const std::string& class_name) {
  return deserializer_map_[class_name];
}
NSClassManager::Serializer& NSClassManager::GetSerializer(const std::string& class_name) {
  return serializer_map_[class_name];
}
