#ifndef NSKEYEDARCHIVER_COMMON_H
#define NSKEYEDARCHIVER_COMMON_H

#include <sys/types.h>

#include <cstdio>

#include "plist/plist.h"

#define DEBUG 1

#define LOG_LEVEL 2
#if LOG_LEVEL >= 0
#define LOG_ERROR(...) printf("ERROR: " __VA_ARGS__)
#else
#define LOG_ERROR(...)
#endif
#if LOG_LEVEL >= 1
#define LOG_INFO(...) printf("INFO: " __VA_ARGS__)
#else
#define LOG_INFO(...)
#endif
#if LOG_LEVEL >= 2
#define LOG_DEBUG(...) printf("DEBUG: " __VA_ARGS__)
#else
#define LOG_DEBUG(...)
#endif

inline void print_plist_as_xml(plist_t node) {
  char* xml = nullptr;
  uint32_t xml_length = 0;
  plist_to_xml(node, &xml, &xml_length);
  if (xml != nullptr) {
    printf("%s\n", xml);
    plist_to_xml_free(xml);
  }
}

#endif  // NSKEYEDARCHIVER_COMMON_H