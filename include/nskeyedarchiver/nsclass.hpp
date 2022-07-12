#ifndef NSKEYEDARCHIVER_NSCLASS_H
#define NSKEYEDARCHIVER_NSCLASS_H

#include <sys/types.h>
#include <string>

namespace nskeyedarchiver {

#define DECLARE_NSCLASS(n)            \
  class n##Class : public NSClass {   \
   public:                            \
    n##Class() : NSClass(#n) {}       \
    virtual NSObject* NewInstance() { \
      n* instance = new n();           \
      instance->SetClass(this);        \
      return instance;                \
    }                                 \
  }

#define IS_A(obj, cls) \
  obj->GetClass() == NSClassFactory::GetInstance().ClassForName(obj->GetClass()->ClassName())

class NSObject;

class NSClass {
 public:
  NSClass(std::string class_name) : class_name_(class_name) {}
  virtual ~NSClass() {}

  virtual NSObject* NewInstance() = 0;

  std::string ClassName() const { return class_name_; }

 private:
  std::string class_name_;
};
using AnyClass = NSClass;

}  // namespace nskeyedarchiver

#endif  // NSKEYEDARCHIVER_NSCLASS_H