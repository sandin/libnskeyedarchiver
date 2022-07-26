#ifndef NSKEYEDARCHIVER_KASTRING_H
#define NSKEYEDARCHIVER_KASTRING_H

#include <map>
#include <string>
#include <vector>

#include "nskeyedarchiver/kaobject.hpp"

namespace nskeyedarchiver {

class KAString : public KAObject {
 public:
  KAString(const std::string& class_name,
           const std::vector<std::string>& classes, const std::string& str)
      : KAObject(class_name, classes), str_(str) {
    LOG_VERBOSE(
        "[%p] KAString(std::string class_name, const char* str), "
        "class_name=%s, str=%s\n",
        this, class_name.c_str(), str.c_str());
  }

  // copy constructor
  KAString(const KAString& other)
      : KAString(other.class_name_, other.classes_, other.str_) {
    LOG_VERBOSE("[%p] KAString(const KAString &other[%p])\n", this, &other);
  }
  // copy assignment operator
  KAString& operator=(const KAString& other) {
    LOG_VERBOSE("[%p] KAString &operator=(const KAString &other[%p)\n", this,
                &other);
    class_name_ = other.class_name_;
    classes_ = other.classes_;
    str_ = other.str_;
    return *this;
  }

  // move constructor
  KAString(KAString&& other)
      : KAObject(std::forward<KAObject>(other)), str_(std::move(other.str_)) {
    LOG_VERBOSE("[%p] KAString(KAString &&other[%p])\n", this, &other);
    // other.str_.clear();
  }

  // move assignment operator
  KAString& operator=(KAString&& other) {
    LOG_VERBOSE("[%p] KAString &operator=(KAString &&other[%p])\n", this,
                &other);
    class_name_ = std::move(other.class_name_);
    classes_ = std::move(other.classes_);
    str_ = std::move(other.str_);
    // other.str_.clear();
    return *this;
  }
  
  virtual bool Equals(const KAString& other) const {
    return class_name_ == other.class_name_ && classes_ == other.classes_ && str_ == other.str_;
  }
  inline bool operator==(const KAString& rhs) { return Equals(rhs); }

  const std::string& ToString() const { return str_; }
  size_t Size() const { return str_.size(); }
  bool Empty() const { return str_.empty(); }

 private:
  std::string str_;
};  // KAString

}  // namespace nskeyedarchiver

#endif  // NSKEYEDARCHIVER_KASTRING_H
