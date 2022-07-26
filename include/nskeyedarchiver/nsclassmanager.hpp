#ifndef NSKEYEDARCHIVER_NS_CLASS_MANAGER_H
#define NSKEYEDARCHIVER_NS_CLASS_MANAGER_H

#include <string>
#include <functional>
#include <map>

#include "nskeyedarchiver/kavalue.hpp"

namespace nskeyedarchiver {

class NSKeyedUnarchiver;

class NSClassManager {
 public:
 //using Serializer = std::function<KAValue(NSKeyedUnarchiver*)>;
 using Deserializer = std::function<KAValue(NSKeyedUnarchiver*)>;

  static NSClassManager& GetInstance() {
    static NSClassManager instance;
    return instance;
  }

  //void RegisterSerializer(std::string class_name, Serializer serializer);
  void RegisterDeserializer(std::string class_name, Deserializer deserializer);

  bool HasDeserializer(std::string class_name);
  Deserializer& GetDeserializer(std::string class_name);
  Deserializer& GetDefaultDeserializer() { return default_deserializer_; }

 private:
  NSClassManager() {}
  ~NSClassManager() {}

  //std::map<std::string, Serializer> serializer_map_;
  std::map<std::string, Deserializer> deserializer_map_;
  NSClassManager::Deserializer default_deserializer_; 

};

}  // namespace nskeyedarchiver

#endif  // NSKEYEDARCHIVER_NS_CLASS_MANAGER_H