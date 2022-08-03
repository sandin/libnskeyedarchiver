#include "nskeyedarchiver/nsclassmanager.hpp"

#include "nskeyedarchiver/nstypes.hpp"

using namespace nskeyedarchiver;

NSClassManager::NSClassManager()
    : default_serializer_(NSDummy::Serialize), default_deserializer_(NSDummy::Deserialize) {
#define REGISTER_SERIALIZERS(name, cls)     \
  RegisterSerializer(name, cls::Serialize); \
  RegisterDeserializer(name, cls::Deserialize);

  RegisterDeserializer("NSDictionary", NSDictionary::Deserialize);
  RegisterDeserializer("NSMutableDictionary", NSDictionary::Deserialize);
  REGISTER_SERIALIZERS("NSArray", NSArray);
  REGISTER_SERIALIZERS("NSMutableArray", NSArray);
  RegisterDeserializer("NSSet", NSArray::Deserialize);
  RegisterDeserializer("NSMutableSet", NSArray::Deserialize);
  RegisterDeserializer("NSString", NSString::Deserialize);
  RegisterDeserializer("NSMutableString", NSString::Deserialize);
  RegisterDeserializer("NSDate", NSDate::Deserialize);
  RegisterDeserializer("NSError", NSError::Deserialize);
  RegisterDeserializer("NSNull", NSNull::Deserialize);

  RegisterDeserializer("DTSysmonTapMessage", DTSysmonTapMessage::Deserialize);
}

NSClassManager::~NSClassManager() {}

void NSClassManager::RegisterDeserializer(const std::string& class_name,
                                          Deserializer deserializer) {
  deserializer_map_[class_name] = deserializer;
}
void NSClassManager::RegisterSerializer(const std::string& class_name, Serializer serializer) {
  serializer_map_[class_name] = serializer;
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
