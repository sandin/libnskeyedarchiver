#include <sys/stat.h>  // stat

#include <cstdio>   // fopen
#include <cstring>  // strcmp
#include <iostream>
#include <string>

#include "nskeyedarchiver/common.hpp"
#include "nskeyedarchiver/nskeyedunarchiver.hpp"

#define USAGE                                              \
  "Usage: nskeyedarchiver_tools <command> <bplist_file>\n" \
  "command:\n"                                             \
  "  bplist2json: convert the input bplist file into json format.\n"

using namespace nskeyedarchiver;

int bplist2json(const char* filename) {
  char* buffer = nullptr;
  size_t buffer_size = 0;
  FILE* f = fopen(filename, "rb");
  if (!f) {
    printf("can not open `%s` file.\n", filename);
    return -1;
  }

  struct stat filestats;
  stat(filename, &filestats);
  buffer_size = filestats.st_size;
  if (buffer_size == 0) {
    printf("can not get `%s` file size.\n", filename);
    return -1;
  }

  buffer = (char*)malloc(buffer_size);
  fread(buffer, 1, buffer_size, f);
  fclose(f);

  if (buffer == nullptr) {
    printf("can not read `%s` file.\n", filename);
    return -1;
  }
  // hexdump(buffer, buffer_size, 0);

  KAValue obj = NSKeyedUnarchiver::UnarchiveTopLevelObjectWithData(buffer, (uint32_t)buffer_size);
  if (obj.IsNull()) {
    printf("Can not parse this keyed archiver.\n");
    return -1;
  }

  std::string json = obj.ToJson();
  printf("%s\n", json.c_str());
  return 0;
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    printf("%s", USAGE);
    return 0;
  }
  char* command = argv[1];
  char* pblist_file = argv[2];
  // printf("command: %s\npblist_file: %s\n", command, pblist_file);

  if (strcmp("bplist2json", command) == 0) {
    return bplist2json(pblist_file);
  } else {
    printf("unsupported command: %s.\n", command);
  }
  return 0;
}
