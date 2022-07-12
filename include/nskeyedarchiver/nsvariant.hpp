#ifndef NSKEYEDARCHIVER_NSVARIANT_H
#define NSKEYEDARCHIVER_NSVARIANT_H

#include <sys/types.h>

#include <string>

#include "nskeyedarchiver/nsclass.hpp"
#include "nskeyedarchiver/nsobject.hpp"

namespace nskeyedarchiver {

class NSVariant : public NSObject {
 public:
  enum class Type {
    Bool,
    UInt,
    Double,
    String,

    LastType
  };

  NSVariant() {}
  NSVariant(bool b) : type_(Type::Bool) { data_.b = b; }
  NSVariant(uint64_t u) : type_(Type::UInt) { data_.u = u; }
  NSVariant(double d) : type_(Type::Double) { data_.d = d; }
  NSVariant(char* s) : type_(Type::String) { data_.s = s; }  // TODO: copy

  virtual ~NSVariant() {
    if (type_ == Type::String && data_.s != nullptr) {
      free(data_.s);
    }
  }

  bool ToBool() const { return data_.b; }
  uint64_t ToUInt() const { return data_.u; }
  double ToDouble() const { return data_.d; }

  Type DataType() const { return type_; }

  virtual bool Decode(NSKeyedUnarchiver* decoder);
  virtual std::string ToString() const;

 private:
  union Data {
    bool b;
    uint64_t u;
    double d;
    char* s;
  } data_;
  Type type_;
};

DECLARE_NSCLASS(NSVariant);

}  // namespace nskeyedarchiver

#endif  // NSKEYEDARCHIVER_NSVARIANT_H