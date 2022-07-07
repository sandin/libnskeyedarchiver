#include "nskeyedarchiver/nskeyedunarchiver.h"

#include "common.h"

using namespace nskeyedarchiver;

NSObject* NSKeyedUnarchiver::Unarchive(const char* data, size_t length) {
  NSObject* obj = nullptr;

  plist_t root_node = nullptr;
  plist_from_bin(data, length, &root_node);
  if (root_node != nullptr) {
#if DEBUG
    print_plist_as_xml(root_node);
#endif

    // TODO
  }
  return obj;
}
