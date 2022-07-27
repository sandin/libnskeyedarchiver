#ifndef NSKEYEDARCHIVER_COMMON_H
#define NSKEYEDARCHIVER_COMMON_H

#include <sys/types.h>

#include <cassert>
#include <cstdio>

#include "plist/plist.h"

#define DEBUG 1

#define LOG_LEVEL 2
#if LOG_LEVEL >= 0
#define LOG_ERROR(fmt, ...) \
  printf("[ERROR] %s:%d:%s(): " fmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
#define LOG_ERROR(fmt, ...)
#endif
#if LOG_LEVEL >= 1
#define LOG_INFO(fmt, ...) \
  printf("[INFO] %s:%d:%s(): " fmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
#define LOG_INFO(fmt, ...)
#endif
#if LOG_LEVEL >= 2
#define LOG_DEBUG(fmt, ...) \
  printf("[DEBUG] %s:%d:%s(): " fmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
#define LOG_DEBUG(fmt, ...)
#endif
#if LOG_LEVEL >= 3
#define LOG_VERBOSE(fmt, ...) \
  printf("[VERBOSE] %s:%d:%s(): " fmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
#define LOG_VERBOSE(fmt, ...)
#endif

#define ASSERT(exp) assert(exp)

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
