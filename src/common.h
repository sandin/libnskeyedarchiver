#ifndef NSKEYEDARCHIVER_COMMON_H
#define NSKEYEDARCHIVER_COMMON_H

#include <sys/types.h>
#include <cstdio>

#include "plist/plist.h"

#define DEBUG 1

inline void print_plist_as_xml(plist_t node)
{
    char *xml = nullptr;
    uint32_t xml_length = 0;
    plist_to_xml(node, &xml, &xml_length);
    if (xml != nullptr)
    {
        printf("%s\n", xml);
        plist_to_xml_free(xml);
    }
}

#endif // NSKEYEDARCHIVER_COMMON_H