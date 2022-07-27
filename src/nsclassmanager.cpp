#include "nskeyedarchiver/nsclassmanager.hpp"

#include "nskeyedarchiver/nstypes.hpp"

using namespace nskeyedarchiver;

NSClassManager::NSClassManager() : default_deserializer_(NSDummy::Deserialize) {
  RegisterDeserializer("NSDictionary", NSDictionary::Deserialize);
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

KAValue NSClassManager::Deserialize(NSKeyedUnarchiver* decoder, const NSClass& clazz) {
  return KAValue();  // TODO: null
}
