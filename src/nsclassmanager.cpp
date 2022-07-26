#include "nskeyedarchiver/nsclassmanager.hpp"

using namespace nskeyedarchiver;

void NSClassManager::RegisterDeserializer(std::string class_name,
                                             Deserializer deserializer) {
  deserializer_map_[class_name] = deserializer;
}

bool NSClassManager::HasDeserializer(std::string class_name) {
  return deserializer_map_.find(class_name) != deserializer_map_.end();
}

NSClassManager::Deserializer& NSClassManager::GetDeserializer(std::string class_name) {
  return deserializer_map_[class_name];
}
