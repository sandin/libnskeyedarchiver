#ifndef NSKEYEDARCHIVER_KAARRAY_H
#define NSKEYEDARCHIVER_KAARRAY_H

#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "nskeyedarchiver/kaobject.hpp"
#include "nskeyedarchiver/kavalue.hpp"

namespace nskeyedarchiver {

class KAArray : public KAObject {
 public:
  using ObjectList = std::vector<KAValue>;

  KAArray(const std::string& class_name, const std::vector<std::string>& classes)
      : KAObject(ArrayKind, class_name, classes) {
    NSKEYEDARCHIVER_LOG_VERBOSE(
        "[%p] KAArray(const std::string& class_name, const ObjectList& arr), "
        "class_name=%s\n",
        this, class_name_.c_str());
  }
  // copy constructor for vector
  KAArray(const std::string& class_name, const std::vector<std::string>& classes,
          const ObjectList& arr)
      : KAObject(ArrayKind, class_name, classes), arr_(arr) {
    NSKEYEDARCHIVER_LOG_VERBOSE(
        "[%p] KAArray(const std::string& class_name, const ObjectList& arr), "
        "class_name=%s\n",
        this, class_name_.c_str());
  }
  // copy constructor for initializer_list
  KAArray(const std::string& class_name, const std::vector<std::string>& classes,
          std::initializer_list<KAValue> list)
      : KAObject(ArrayKind, class_name, classes), arr_(list) {
    NSKEYEDARCHIVER_LOG_VERBOSE(
        "[%p] KAArray(const std::string& class_name, "
        "std::initializer_list<KVValue> list), class_name=%s\n",
        this, class_name_.c_str());
  }
  // move constructor for vector
  KAArray(const std::string& class_name, const std::vector<std::string>& classes, ObjectList&& arr)
      : KAObject(ArrayKind, class_name, classes), arr_(std::forward<ObjectList>(arr)) {
    NSKEYEDARCHIVER_LOG_VERBOSE(
        "[%p] KAArray(std::string class_name, ObjectList&& arr), "
        "class_name=%s\n",
        this, class_name_.c_str());
  }

  // copy constructor
  KAArray(const KAArray& other) : KAArray(other.class_name_, other.classes_, other.arr_) {
    NSKEYEDARCHIVER_LOG_VERBOSE("[%p] KAArray(const KAObject &other[%p])\n", this, &other);
  }
  // copy assignment operator
  KAArray& operator=(const KAArray& other) {
    NSKEYEDARCHIVER_LOG_VERBOSE("[%p] KAArray &operator=(const KAArray &other[%p)\n", this, &other);
    class_name_ = other.class_name_;
    classes_ = other.classes_;
    arr_ = other.arr_;
    return *this;
  }

  // move constructor
  KAArray(KAArray&& other) : KAObject(std::forward<KAObject>(other)), arr_(std::move(other.arr_)) {
    NSKEYEDARCHIVER_LOG_VERBOSE("[%p] KAArray(KAArray &&other[%p])\n", this, &other);
    // other.arr_.clear()
  }
  // move assignment operator
  KAArray& operator=(KAArray&& other) {
    NSKEYEDARCHIVER_LOG_VERBOSE("[%p] KAArray &operator=(KAArray &&other[%p])\n", this, &other);
    class_name_ = std::move(other.class_name_);
    classes_ = std::move(other.classes_);
    arr_ = std::move(other.arr_);
    // other.arr_.clear();
    return *this;
  }

  virtual KAArray* Clone() const override { return new KAArray(*this); }
  virtual KAArray* CloneByMove(KAObject&& other) const override {
    NSKEYEDARCHIVER_ASSERT(other.GetKind() == ArrayKind, "can not copy a difference kind object.\n");
    return new KAArray(std::move(static_cast<KAArray&&>(other)));
  }

  virtual bool Equals(const KAObject& other) const override {
    const KAArray&& o = static_cast<const KAArray&&>(other);
    return kind_ == o.kind_ && class_name_ == o.class_name_ && classes_ == o.classes_ &&
           arr_ == o.arr_;
  }
  inline bool operator==(const KAArray& rhs) { return Equals(rhs); }

  virtual std::string ToJson() const override {
    std::stringstream ss;
    ss << "[";

    int i = 0;
    size_t size = arr_.size();
    for (const KAValue& item : arr_) {
      ss << item.ToJson();
      if (i < size - 1) {
        ss << ",";
      }
      i++;
    }
    ss << "]";
    return ss.str();
  }

  // arr[i] = value
  const KAValue& operator[](size_t idx) const { return arr_[idx]; }

  // at(key)
  KAValue& at(size_t key) { return arr_.at(key); }
  const KAValue& at(size_t key) const { return arr_.at(key); }

  void push_back(const KAValue& value) { arr_.push_back(value); }
  void push_back(KAValue&& value) {
    arr_.push_back(std::forward<KAValue>(value));
    // value.clear();
  }

  const ObjectList& ToArray() const { return arr_; }
  size_t Size() const { return arr_.size(); }
  bool Empty() const { return arr_.empty(); }

 private:
  ObjectList arr_;
};  // KAArray

}  // namespace nskeyedarchiver

#endif  // NSKEYEDARCHIVER_KAARRAY_H
