#ifndef NSKEYEDARCHIVER_KAOBJECT_H
#define NSKEYEDARCHIVER_KAOBJECT_H

#include <map>
#include <string>
#include <vector>

#include "nskeyedarchiver/common.hpp"

namespace nskeyedarchiver {

class KAObject {
 public:
  KAObject(const std::string& class_name, const std::vector<std::string>& classes)
      : class_name_(class_name), classes_(classes) {
    LOG_VERBOSE(
        "[%p] KAObject(const std::string& class_name, const "
        "std::vector<std::string>& classes), "
        "class_name=%s\n",
        this, class_name.c_str());
  }

  // copy constructor
  KAObject(const KAObject& other) : class_name_(other.class_name_), classes_(other.classes_) {
    LOG_VERBOSE("[%p] KAObject(const KAObject &other[%p])\n", this, &other);
  }
  // copy assignment operator
  KAObject& operator=(const KAObject& other) {
    LOG_VERBOSE("[%p] KAObject &operator=(const KAObject &other[%p)\n", this, &other);
    class_name_ = other.class_name_;
    classes_ = other.classes_;
    return *this;
  }

  // move constructor
  KAObject(KAObject&& other)
      : class_name_(std::move(other.class_name_)), classes_(std::move(other.classes_)) {
    LOG_VERBOSE("[%p] KAObject(KAObject &&other[%p])\n", this, &other);
  }
  // move assignment operator
  KAObject& operator=(KAObject&& other) {
    LOG_VERBOSE("[%p] KAObject &operator=(KAObject &&other[%p])\n", this, &other);
    class_name_ = std::move(other.class_name_);
    classes_ = std::move(other.classes_);
    return *this;
  }

  virtual KAObject* Clone() const { return new KAObject(*this); }
  virtual KAObject* CloneByMove(KAObject&& other) const { return new KAObject(std::move(other)); }

  virtual bool Equals(const KAObject& other) {
    return class_name_ == other.class_name_ && classes_ == other.classes_;
  }
  inline bool operator==(const KAObject& rhs) { return Equals(rhs); }

  virtual ~KAObject() { LOG_VERBOSE("[%p] ~KAObject, class_name=%s\n", this, class_name_.c_str()); }

  const std::string& ClassName() const { return class_name_; }
  const std::vector<std::string>& Classes() const { return classes_; }

  bool IsA(const std::string& class_name) { return class_name == class_name_; }
  bool IsInstanceOf(const std::string& class_name) {
    for (auto& cls : classes_) {
      if (cls == class_name) {
        return true;
      }
    }
    return false;
  }

 protected:
  std::string class_name_;
  std::vector<std::string> classes_;

  bool empty_;
};  // KAObject

}  // namespace nskeyedarchiver

#endif  // NSKEYEDARCHIVER_KAOBJECT_H
