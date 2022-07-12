#include "nskeyedarchiver/nsclassfactory.hpp"

#include "nskeyedarchiver/nsset.hpp"
#include "nskeyedarchiver/nsarray.hpp"
#include "nskeyedarchiver/nsstring.hpp"
#include "nskeyedarchiver/nsvariant.hpp"
#include "nskeyedarchiver/nsdictionary.hpp"

using namespace nskeyedarchiver;

// static

#define REGISTER_CLASS(n)                                            \
  {                                                                  \
    n##Class* clazz = new n##Class();                                \
    RegisterClass(clazz->ClassName(), static_cast<NSClass*>(clazz)); \
  }

NSClassFactory::NSClassFactory() { RegisterBuiltInClasses(); }

NSClassFactory::~NSClassFactory() {
  // TODO: free classes
}

void NSClassFactory::RegisterBuiltInClasses() {
  REGISTER_CLASS(NSVariant);
  REGISTER_CLASS(NSDictionary);
  REGISTER_CLASS(NSMutableDictionary);
  REGISTER_CLASS(NSArray);
  REGISTER_CLASS(NSSet);
  REGISTER_CLASS(NSMutableSet);
  REGISTER_CLASS(NSString);
  REGISTER_CLASS(NSMutableString);
}

void NSClassFactory::RegisterClass(std::string class_name, NSClass* coding) {
  classes_map_[class_name] = coding;
}

void NSClassFactory::UnregisterClass(std::string class_name) {
  auto found = classes_map_.find(class_name);
  if (found != classes_map_.end()) {
    delete found->second;
    classes_map_.erase(found);
  }
}

NSClass* NSClassFactory::ClassForName(std::string class_name) {
  auto found = classes_map_.find(class_name);
  if (found != classes_map_.end()) {
    return found->second;
  }
  return nullptr;
}