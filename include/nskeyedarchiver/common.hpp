#ifndef NSKEYEDARCHIVER_COMMON_H
#define NSKEYEDARCHIVER_COMMON_H

#include <sys/types.h>

#include <cassert>
#include <cstdio>

#include "plist/plist.h"

#ifdef NDEBUG
#define DEBUG 0
#define LOG_LEVEL 0
#else
#define DEBUG 1
#define LOG_LEVEL 2
#endif

#if LOG_LEVEL >= 0
#define LOG_ERROR(fmt, ...) \
  fprintf(stderr, "[ERROR] %s:%d:%s(): " fmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
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

#define ASSERT(exp, fmt, ...) if (!exp) { LOG_ERROR(fmt, ##__VA_ARGS__); } assert(exp)

inline void print_plist_as_xml(plist_t node) {
  char* xml = nullptr;
  uint32_t xml_length = 0;
  plist_to_xml(node, &xml, &xml_length);
  if (xml != nullptr) {
    printf("%s\n", xml);
    plist_to_xml_free(xml);
  }
}

inline void hexdump(void* addr, int len, int offset) {
  int i;
  unsigned char buff[17];
  unsigned char* pc = (unsigned char*)addr;

  // Process every byte in the data.
  for (i = 0; i < len; i++) {
    // Multiple of 16 means new line (with line offset).

    if ((i % 16) == 0) {
      // Just don't print ASCII for the zeroth line.
      if (i != 0) printf("  %s\n", buff);

      // Output the offset.
      printf("  %08x ", i + offset);
    }

    // Now the hex code for the specific character.
    printf(" %02x", pc[i]);

    // And store a printable ASCII character for later.
    if ((pc[i] < 0x20) || (pc[i] > 0x7e)) {
      buff[i % 16] = '.';
    } else {
      buff[i % 16] = pc[i];
    }

    buff[(i % 16) + 1] = '\0';
  }

  // Pad out last line if not exactly 16 characters.
  while ((i % 16) != 0) {
    printf("   ");
    i++;
  }

  // And print the final ASCII bit.
  printf("  %s\n", buff);
}

#endif  // NSKEYEDARCHIVER_COMMON_H
