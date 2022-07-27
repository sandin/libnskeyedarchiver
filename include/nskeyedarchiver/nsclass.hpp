#ifndef NSKEYEDARCHIVER_NS_CLASS_H
#define NSKEYEDARCHIVER_NS_CLASS_H

namespace nskeyedarchiver {

struct NSClass {
  std::string class_name;
  std::vector<std::string> classes;
  std::vector<std::string> classhints;
};

}  // namespace nskeyedarchiver

#endif  // NSKEYEDARCHIVER_NS_CLASS_H
