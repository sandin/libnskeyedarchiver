#ifndef NSKEYEDARCHIVER_NS_CLASS_MANAGER_H
#define NSKEYEDARCHIVER_NS_CLASS_MANAGER_H

#include <functional>
#include <map>
#include <string>
#include <vector>

#include "nskeyedarchiver/kavalue.hpp"
#include "nskeyedarchiver/nsclass.hpp"

namespace nskeyedarchiver {

class NSKeyedUnarchiver;

class NSClassManager {
 public:
  // using Serializer = std::function<KAValue(NSKeyedUnarchiver*, const NSClass&
  // clazz)>;
  using Deserializer = std::function<KAValue(NSKeyedUnarchiver*, const NSClass& clazz)>;

  static NSClassManager& GetInstance() {
    static NSClassManager instance;
    return instance;
  }

  // void RegisterSerializer(std::string class_name, Serializer serializer);
  void RegisterDeserializer(const std::string& class_name, Deserializer deserializer);

  bool HasDeserializer(const std::string& class_name);
  Deserializer& GetDeserializer(const std::string& class_name);
  Deserializer& GetDefaultDeserializer() { return default_deserializer_; }

 private:
  NSClassManager();
  virtual ~NSClassManager();

  // std::map<std::string, Serializer> serializer_map_;
  std::map<std::string, Deserializer> deserializer_map_;
  NSClassManager::Deserializer default_deserializer_;
};

}  // namespace nskeyedarchiver

#endif  // NSKEYEDARCHIVER_NS_CLASS_MANAGER_H
