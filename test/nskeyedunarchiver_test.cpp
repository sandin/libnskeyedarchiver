#include "nskeyedarchiver/nskeyedunarchiver.hpp"

#include <gtest/gtest.h>
#include <sys/stat.h>

using namespace nskeyedarchiver;

void read_content_from_file(const char* filename, char** buffer,
                            size_t* buffer_size) {
  FILE* f = fopen(filename, "rb");
  if (!f) {
    return;
  }
  struct stat filestats;
  stat(filename, &filestats);
  *buffer_size = filestats.st_size;
  *buffer = (char*)malloc(*buffer_size);
  fread(*buffer, 1, *buffer_size, f);
  fclose(f);
}

// Demonstrate some basic assertions.
TEST(NSKeyedUnarchiverTest, UnarchiveTopLevelObjectWithData) {
  char* buffer = nullptr;
  size_t buffer_size = 0;
  const char* filename = "../test/data/setconfig.bplist";
  read_content_from_file(filename, &buffer, &buffer_size);
  if (buffer == nullptr) {
    printf("can not read %s file\n", filename);
    EXPECT_TRUE(false);
    return;
  }
  printf("buffer=0x%p, buffer_size=%zu\n", static_cast<void*>(buffer), buffer_size);
  EXPECT_EQ(1032, buffer_size);

  NSObject* obj = NSKeyedUnarchiver::UnarchiveTopLevelObjectWithData(
      buffer, (uint32_t)buffer_size);
  EXPECT_TRUE(obj != nullptr);

  if (obj) {
    delete obj;
  }
}