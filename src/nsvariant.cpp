#include "nskeyedarchiver/nsvariant.hpp"

using namespace nskeyedarchiver;

bool NSVariant::Decode(NSKeyedUnarchiver* decoder) {
  // pass
  return true;
}

std::string NSVariant::ToString() const {
  switch (type_) {
    case Type::Bool:
      return std::to_string(data_.b);
    case Type::UInt:
      return std::to_string(data_.u);
    case Type::Double:
      return std::to_string(data_.d);
    case Type::String:
      return std::string("\"") + std::string(data_.s) + std::string("\"");
    default:
      return "";
  };
}