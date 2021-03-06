#ifndef NSKEYEDARCHIVER_KAMAP_H
#define NSKEYEDARCHIVER_KAMAP_H

#include <map>
#include <string>
#include <vector>

#include "nskeyedarchiver/kaobject.hpp"
#include "nskeyedarchiver/kavalue.hpp"

namespace nskeyedarchiver {

class KAMap : public KAObject {
 public:
  using KeyType = std::string;
  using ObjectMap = std::map<KeyType, KAValue>;

  KAMap(const std::string& class_name, const std::vector<std::string>& classes)
      : KAObject(class_name, classes) {
    LOG_VERBOSE(
        "[%p] KAMap(const std::string& class_name), "
        "class_name=%s\n",
        this, class_name_.c_str());
  }
  // copy constructor for map
  KAMap(const std::string& class_name, const std::vector<std::string>& classes,
        const ObjectMap& map)
      : KAObject(class_name, classes), map_(map) {
    LOG_VERBOSE(
        "[%p] KAMap(const std::string& class_name, const ObjectMap& map), "
        "class_name=%s\n",
        this, class_name_.c_str());
  }
  // move constructor for map
  KAMap(const std::string& class_name, const std::vector<std::string>& classes, ObjectMap&& map)
      : KAObject(class_name, classes), map_(std::forward<ObjectMap>(map)) {
    LOG_VERBOSE(
        "[%p] KAMap(const std::string& class_name, ObjectMap &&map), "
        "class_name=%s\n",
        this, class_name_.c_str());
  }

  // copy constructor
  KAMap(const KAMap& other) : KAMap(other.class_name_, other.classes_, other.map_) {
    LOG_VERBOSE("[%p] KAMap(const KAMap &other[%p])\n", this, &other);
  }
  // copy assignment operator
  KAMap& operator=(const KAMap& other) {
    LOG_VERBOSE("[%p] KAMap &operator=(const KAMap &other[%p)\n", this, &other);
    class_name_ = other.class_name_;
    classes_ = other.classes_;
    map_ = other.map_;
    return *this;
  }

  // move constructor
  KAMap(KAMap&& other) : KAObject(std::forward<KAObject>(other)), map_(std::move(other.map_)) {
    LOG_VERBOSE("[%p] KAMap(KAMap &&other[%p])\n", this, &other);
    // other.map_.clear();
  }
  // move assignment operator
  KAMap& operator=(KAMap&& other) {
    LOG_VERBOSE("[%p] KAMap &operator=(KAMap &&other[%p])\n", this, &other);
    class_name_ = std::move(other.class_name_);
    classes_ = std::move(other.classes_);
    map_ = std::move(other.map_);
    // other.map_.clear();
    return *this;
  }

  // map[key] copy
  KAValue& operator[](const KeyType& key) {
    LOG_VERBOSE("[%p] KAValue &operator[](const std::string &key), key=%s\n", this, key.c_str());
    return map_[key];
  }
  // map[key] move
  KAValue& operator[](KeyType&& key) { return map_[std::forward<KeyType>(key)]; }

  // at(key)
  KAValue& at(const KeyType& key) { return map_.at(key); }
  const KAValue& at(const KeyType& key) const { return map_.at(key); }

  virtual KAMap* Clone() const { return new KAMap(*this); }
  virtual KAMap* CloneByMove(KAMap&& other) const { return new KAMap(std::move(other)); }

  virtual bool Equals(const KAMap& other) const {
    return class_name_ == other.class_name_ && classes_ == other.classes_ && map_ == other.map_;
  }
  inline bool operator==(const KAMap& rhs) { return Equals(rhs); }

  virtual std::string ToJson() const {
    std::stringstream ss;
    ss << "{";
    size_t size = map_.size();
    int i = 0;
    for (const auto& kv : map_) {
      ss << "\"" << kv.first << "\"";
      ss << ":";
      ss << kv.second.ToJson();
      if (i < size - 1) {
        ss << ",";
      }
      i++;
    }
    ss << "}";
    return ss.str();
  }

  ObjectMap& ToMap() { return map_; }
  size_t Size() const { return map_.size(); }
  bool Empty() const { return map_.empty(); }

 private:
  ObjectMap map_;
};  // KAMap

}  // namespace nskeyedarchiver

#endif  // NSKEYEDARCHIVER_KAMAP_H
