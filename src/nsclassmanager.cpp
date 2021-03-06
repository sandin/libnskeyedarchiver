#include "nskeyedarchiver/nsclassmanager.hpp"

#include "nskeyedarchiver/nstypes.hpp"

using namespace nskeyedarchiver;

NSClassManager::NSClassManager() : default_deserializer_(NSDummy::Deserialize) {
  RegisterDeserializer("NSDictionary", NSDictionary::Deserialize);
  RegisterDeserializer("NSMutableDictionary", NSDictionary::Deserialize);
  RegisterDeserializer("NSArray", NSArray::Deserialize);
  RegisterDeserializer("NSMutableArray", NSArray::Deserialize);
  RegisterDeserializer("NSSet", NSArray::Deserialize);
  RegisterDeserializer("NSMutableSet", NSArray::Deserialize);
  RegisterDeserializer("NSString", NSString::Deserialize);
  RegisterDeserializer("NSMutableString", NSString::Deserialize);
  RegisterDeserializer("NSDate", NSDate::Deserialize);
  RegisterDeserializer("NSNull", NSNull::Deserialize);
}

NSClassManager::~NSClassManager() {}

void NSClassManager::RegisterDeserializer(const std::string& class_name,
                                          Deserializer deserializer) {
  deserializer_map_[class_name] = deserializer;
}

bool NSClassManager::HasDeserializer(const std::string& class_name) {
  return deserializer_map_.find(class_name) != deserializer_map_.end();
}

NSClassManager::Deserializer& NSClassManager::GetDeserializer(const std::string& class_name) {
  return deserializer_map_[class_name];
}
