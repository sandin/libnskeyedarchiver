#ifndef NSKEYEDARCHIVER_NSCLASS_FACTORY_H
#define NSKEYEDARCHIVER_NSCLASS_FACTORY_H

#include <sys/types.h>

#include <string>
#include <unordered_map>

#include "nskeyedarchiver/nsclass.hpp"
#include "nskeyedarchiver/nscoding.hpp"

namespace nskeyedarchiver {

class NSClassFactory {
 public:
  NSClassFactory();
  virtual ~NSClassFactory();

  static NSClassFactory& GetInstance() {
    static NSClassFactory instance;
    return instance;
  }

  void RegisterClass(std::string class_name, NSClass* coding);
  void UnregisterClass(std::string class_name);

  NSClass* ClassForName(std::string class_name);

 private:
  void RegisterBuiltInClasses();

  std::unordered_map<std::string, NSClass*> classes_map_;
};

}  // namespace nskeyedarchiver

#endif  // NSKEYEDARCHIVER_NSCLASS_FACTORY_H