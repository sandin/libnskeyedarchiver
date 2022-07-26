#ifndef NSKEYEDARCHIVER_KAVALUE_H
#define NSKEYEDARCHIVER_KAVALUE_H

#include <sstream>
#include <cstring> // strcmp

#include "nskeyedarchiver/common.h"
#include "nskeyedarchiver/kaobject.hpp"

namespace nskeyedarchiver {


class KAValue {
 public:
  enum DataType { Null, Bool, Integer, Double, Str, Object };

  union Data {
    bool b;
    uint64_t u;
    double d;
    char* s;

    KAObject* o;
  };

  KAValue() : t_(DataType::Null) {}

  explicit KAValue(uint64_t u) : t_(DataType::Integer) {
    LOG_VERBOSE("[%p] KAValue(uint64 u), u=%llu\n", this, u);
    d_.u = u;
  }
  KAValue& operator=(uint64_t u) {
    LOG_VERBOSE("[%p] KAValue &operator=(uint64_t u), u=%llu\n", this, u);
    t_ = DataType::Integer;
    d_.u = u;
    return *this;
  }
  explicit KAValue(uint32_t u) : KAValue(static_cast<uint64_t>(u)) {}
  KAValue& operator=(uint32_t u) { return operator=(static_cast<uint64_t>(u)); }

  explicit KAValue(int64_t u) : KAValue(static_cast<uint64_t>(u)) {}
  KAValue& operator=(int64_t u) { return operator=(static_cast<uint64_t>(u)); }

  explicit KAValue(int32_t u) : KAValue(static_cast<uint64_t>(u)) {}
  KAValue& operator=(int32_t u) { return operator=(static_cast<uint64_t>(u)); }

  explicit KAValue(bool b) : t_(DataType::Bool) {
    LOG_VERBOSE("[%p] KAValue(bool b), b=%d\n", this, b);
    d_.b = b;
  }
  KAValue& operator=(bool b) {
    LOG_VERBOSE("[%p] KAValue &operator=(bool b), b=%d\n", this, b);
    t_ = DataType::Bool;
    d_.b = b;
    return *this;
  }

  explicit KAValue(double d) : t_(DataType::Double) {
    LOG_VERBOSE("[%p] KAValue(double d), d=%f\n", this, d);
    d_.d = d;
  }
  KAValue& operator=(double d) {
    LOG_VERBOSE("[%p] KAValue& operator=(double d), d=%f\n", this, d);
    t_ = DataType::Double;
    d_.d = d;
    return *this;
  }

  explicit KAValue(const char* s) : t_(DataType::Str) {
    LOG_VERBOSE("[%p] KAValue(const char * s), s=\"%s\"\n", this, s);
    d_.s = strdup(s);
  }
  KAValue& operator=(const char* s) {
    LOG_VERBOSE("[%p] KAValue &operator=(const char * s), s=\"%s\")\n", this,
                s);
    t_ = DataType::Str;
    d_.s = strdup(s);
    return *this;
  }

  // copy object
  explicit KAValue(const KAObject& object) : t_(DataType::Object) {
    LOG_VERBOSE("[%p] KAValue(const KAObject &object)\n", this);
    d_.o = new KAObject(object);
  }
  // copy assignment operator(for KAObject)
  KAValue& operator=(const KAObject& object) {
    LOG_VERBOSE("[%p] KAValue &operator=(const KAObject &object)\n", this);
    t_ = DataType::Object;
    d_.o = new KAObject(object);
    return *this;
  }
  // move object
  KAValue(KAObject&& object) : t_(DataType::Object) {
    LOG_VERBOSE("[%p] KAValue(KAObject &&object)\n", this);
    d_.o = new KAObject(std::forward<KAObject>(object));
  }
  // move assignment operator（for KAObject)
  KAValue& operator=(KAObject&& object) {
    LOG_VERBOSE("[%p] KAValue &operator=(KAObject &&object)\n", this);
    t_ = DataType::Object;
    d_.o = new KAObject(std::forward<KAObject>(object));
    // TODO: reset object
    return *this;
  }

  // copy constructor
  KAValue(const KAValue& other) : t_(other.t_) {
    LOG_VERBOSE("[%p] KAValue(const KAValue &other), other=[%p]\n", this,
                &other);
    if (t_ == DataType::Str) {
      d_.s = strdup(other.d_.s);  // copy char*
    } else if (t_ == DataType::Object) {
      d_.o = new KAObject(*other.d_.o); // copy object*
    } else {
      d_ = other.d_;
    }
  }
  // copy assignment operator
  KAValue& operator=(const KAValue& other) {
    LOG_VERBOSE("[%p] KAValue &operator=(const KAValue &other), other=[%p]\n",
                this, &other);
    t_ = other.t_;
    if (t_ == DataType::Str) {
      d_.s = strdup(other.d_.s);  // copy char*
    } else if (t_ == DataType::Object) {
      d_.o = new KAObject(*other.d_.o); // copy object*
    } else {
      d_ = other.d_;
    }
    return *this;
  }
  // move constructor
  KAValue(KAValue&& other) : t_(other.t_) {
    LOG_VERBOSE("[%p] KAValue(KAValue &&other), other=[%p]\n", this, &other);
    if (t_ == DataType::Str) {
      d_.s = other.d_.s;  // move char*
      other.d_.s = nullptr;
    } else if (t_ == DataType::Object) {
      d_.o = other.d_.o; // move object*
      other.d_.o = nullptr;
    } else {
      d_ = other.d_;
    }
    other.t_ = DataType::Null;
  }
  // move assignment operator
  KAValue& operator=(KAValue&& other) {
    LOG_VERBOSE("[%p] KAValue &operator=(KAValue &&other), other=[%p]\n", this,
                &other);
    t_ = other.t_;
    if (t_ == DataType::Str) {
      d_.s = other.d_.s;  // move char*
      other.d_.s = nullptr;
    } else if (t_ == DataType::Object) {
      d_.o = other.d_.o; // move object*
      other.d_.o = nullptr;
    } else {
      d_ = other.d_;
    }
    other.t_ = DataType::Null;
    return *this;
  }
  
  virtual bool Equals(const KAValue& other) const {
    if (t_ != other.t_) {
      return false;
    }
    
    switch (t_) {
      case DataType::Bool:
        return d_.b == other.d_.b;
        break;
      case DataType::Integer:
        return d_.u == other.d_.u;
      case DataType::Double:
        return d_.d == other.d_.d;
      case DataType::Str:
        return strcmp(d_.s, other.d_.s) == 0;
      case DataType::Object:
        return d_.o->Equals(*other.d_.o);
      default:
        return false;
    }
  }
  inline bool operator==(const KAValue& rhs) { return Equals(rhs); }

  ~KAValue() {
    LOG_VERBOSE("[%p] ~KAValue, %s\n", this, Dump().c_str());
    if (t_ == DataType::Str) {
      if (d_.s) {
        free(d_.s);
      }
    } else if (t_ == DataType::Object) {
      if (d_.o) {
        delete d_.o;
      }
    }
  }

  std::string Dump() {
    std::stringstream ss;
    switch (t_) {
      case DataType::Bool:
        ss << "t=Bool, d=" << d_.b;
        break;
      case DataType::Integer:
        ss << "t=Integer, d=" << d_.u;
        break;
      case DataType::Double:
        ss << "t=Double, d=" << d_.d;
        break;
      case DataType::Str:
        ss << "t=Str, d=\"" << d_.s << "\"";
        break;
      case DataType::Object:
        ss << "t=Object, d=" << d_.o->ClassName();
        break;
      default:
        ss << "t=" << t_;
        break;
    }
    return ss.str();
  }

  uint64_t ToInteger() const { return d_.u; }
  bool ToBool() const { return d_.b; }
  double ToDouble() const { return d_.d; }
  const char* ToStr() const { return d_.s; }
  const KAObject& ToObject() const { return *d_.o; }

  bool IsNull() const { return t_ == DataType::Null; }
  bool IsInteger() const { return t_ == DataType::Integer; }
  bool IsBool() const { return t_ == DataType::Bool; }
  bool IsDouble() const { return t_ == DataType::Double; }
  bool IsStr() const { return t_ == DataType::Str; }
  bool IsObject() const { return t_ == DataType::Object; }

 private:
  Data d_;
  DataType t_;
}; // KAValue

constexpr bool operator==(const KAValue& lhs, const KAValue& rhs) {
    return lhs.Equals(rhs);
}

}  // namespace nskeyedarchiver

#endif  // NSKEYEDARCHIVER_KAVALUE_H
