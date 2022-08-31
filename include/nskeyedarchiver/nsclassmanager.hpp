#ifndef NSKEYEDARCHIVER_NS_CLASS_MANAGER_H
#define NSKEYEDARCHIVER_NS_CLASS_MANAGER_H

#include <functional>
#include <map>
#include <string>
#include <vector>

#include "nskeyedarchiver/kavalue.hpp"
#include "nskeyedarchiver/nsclass.hpp"
#include "plist/plist.h"

namespace nskeyedarchiver {

class NSKeyedArchiver;
class NSKeyedUnarchiver;

class NSClassManager {
 public:
  using Serializer =
      std::function<bool(NSKeyedArchiver*, const NSClass& clazz, const KAValue& object)>;
  using Deserializer = std::function<KAValue(NSKeyedUnarchiver*, const NSClass& clazz)>;

  static NSClassManager& GetInstance() {
    static NSClassManager instance;
    return instance;
  }

  void RegisterSerializer(const std::string& class_name, Serializer serializer);
  void UnregisterSerializer(const std::string& class_name);
  void RegisterDeserializer(const std::string& class_name, Deserializer deserializer);
  void UnregisterDeserializer(const std::string& class_name);

  bool HasSerializer(const std::string& class_name);
  bool HasDeserializer(const std::string& class_name);

  Serializer& GetSerializer(const std::string& class_name);
  Deserializer& GetDeserializer(const std::string& class_name);

  Serializer& GetDefaultSerializer() { return default_serializer_; }
  Deserializer& GetDefaultDeserializer() { return default_deserializer_; }

 private:
  NSClassManager();
  virtual ~NSClassManager();

  std::map<std::string, Serializer> serializer_map_;
  std::map<std::string, Deserializer> deserializer_map_;
  NSClassManager::Serializer default_serializer_;
  NSClassManager::Deserializer default_deserializer_;
};

}  // namespace nskeyedarchiver

#endif  // NSKEYEDARCHIVER_NS_CLASS_MANAGER_H
