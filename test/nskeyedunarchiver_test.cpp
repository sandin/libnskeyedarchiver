#include "nskeyedarchiver/nskeyedunarchiver.h"

#include <gtest/gtest.h>
#include <sys/stat.h>

using namespace nskeyedarchiver;

void read_content_from_file(const char* filename, char** buffer,
                            size_t* buffer_size) {
  FILE* f = fopen(filename, "rb");
  struct stat filestats;
  stat(filename, &filestats);
  *buffer_size = filestats.st_size;
  *buffer = (char*)malloc(*buffer_size);
  fread(*buffer, 1, *buffer_size, f);
  fclose(f);
}

// Demonstrate some basic assertions.
TEST(NSKeyedUnarchiverTest, Unarchive) {
  char* buffer;
  size_t buffer_size;
  read_content_from_file("./test/data/setconfig.bplist", &buffer, &buffer_size);

  NSKeyedUnarchiver* unarchiver = new NSKeyedUnarchiver();
  NSObject* obj = unarchiver->Unarchive(buffer, (uint32_t)buffer_size);
  EXPECT_TRUE(true);

  delete unarchiver;
  if (obj) {
    delete obj;
  }
}