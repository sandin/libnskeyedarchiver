#ifndef NSKEYEDARCHIVER_KAOBJECT_H
#define NSKEYEDARCHIVER_KAOBJECT_H

#include <map>
#include <string>
#include <vector>

#include "nskeyedarchiver/common.hpp"

namespace nskeyedarchiver {

class KAObject {
 public:
  enum Kind { InvalidKind, ObjectKind, MapKind, ArrayKind };

  KAObject(Kind kind, const std::string& class_name, const std::vector<std::string>& classes)
      : kind_(kind), class_name_(class_name), classes_(classes) {
    NSKEYEDARCHIVER_LOG_VERBOSE(
        "[%p] KAObject(Kind kind, const std::string& class_name, const "
        "std::vector<std::string>& classes), kind=%d, "
        "class_name=%s\n",
        this, kind, class_name.c_str());
  }
  KAObject(const std::string& class_name, const std::vector<std::string>& classes)
      : KAObject(ObjectKind, class_name, classes) {}

  // copy constructor
  KAObject(const KAObject& other)
      : kind_(ObjectKind), class_name_(other.class_name_), classes_(other.classes_) {
    NSKEYEDARCHIVER_LOG_VERBOSE("[%p] KAObject(const KAObject &other[%p])\n", this, &other);
  }
  // copy assignment operator
  KAObject& operator=(const KAObject& other) {
    NSKEYEDARCHIVER_LOG_VERBOSE("[%p] KAObject &operator=(const KAObject &other[%p)\n", this, &other);
    class_name_ = other.class_name_;
    classes_ = other.classes_;
    return *this;
  }

  // move constructor
  KAObject(KAObject&& other)
      : kind_(other.kind_),
        class_name_(std::move(other.class_name_)),
        classes_(std::move(other.classes_)) {
    NSKEYEDARCHIVER_LOG_VERBOSE("[%p] KAObject(KAObject &&other[%p])\n", this, &other);
  }
  // move assignment operator
  KAObject& operator=(KAObject&& other) {
    NSKEYEDARCHIVER_LOG_VERBOSE("[%p] KAObject &operator=(KAObject &&other[%p])\n", this, &other);
    class_name_ = std::move(other.class_name_);
    classes_ = std::move(other.classes_);
    return *this;
  }

  virtual KAObject* Clone() const { return new KAObject(*this); }
  virtual KAObject* CloneByMove(KAObject&& other) const { return new KAObject(std::move(other)); }

  virtual bool Equals(const KAObject& other) const {
    return kind_ == other.kind_ && class_name_ == other.class_name_ && classes_ == other.classes_;
  }
  inline bool operator==(const KAObject& rhs) { return Equals(rhs); }

  virtual std::string ToJson() const { return "null"; }

  virtual ~KAObject() { NSKEYEDARCHIVER_LOG_VERBOSE("[%p] ~KAObject, class_name=%s\n", this, class_name_.c_str()); }

  const std::string& ClassName() const { return class_name_; }
  const std::vector<std::string>& Classes() const { return classes_; }
  Kind GetKind() const { return kind_; }
  bool IsA(Kind kind) const { return kind == kind_; }

 protected:
  const Kind kind_;

  std::string class_name_;
  std::vector<std::string> classes_;
};  // KAObject

}  // namespace nskeyedarchiver

#endif  // NSKEYEDARCHIVER_KAOBJECT_H
