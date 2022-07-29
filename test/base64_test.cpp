#include "nskeyedarchiver/base64.hpp"

#include <gtest/gtest.h>
#include <sys/stat.h>

using namespace nskeyedarchiver;

TEST(Base64Test, Encode) {
  unsigned char buffer0[] = {};
  ASSERT_STREQ("", base64encode((const char*)&buffer0[0], (size_t)sizeof(buffer0)).c_str());
  
  unsigned char buffer1[] = { 0x14, 0xfb, 0x9c, 0x03, 0xd9, 0x7e };
  ASSERT_STREQ("FPucA9l+", base64encode((const char*)&buffer1[0], (size_t)sizeof(buffer1)).c_str());
  
  unsigned char buffer2[] = { 0x14, 0xfb, 0x9c, 0x03, 0xd9 };
  ASSERT_STREQ("FPucA9k=", base64encode((const char*)&buffer2[0], (size_t)sizeof(buffer2)).c_str());
  
  unsigned char buffer3[] = { 0x14, 0xfb, 0x9c, 0x03 };
  ASSERT_STREQ("FPucAw==", base64encode((const char*)&buffer3[0], (size_t)sizeof(buffer3)).c_str());
  
  const char* buffer4 = R"(I'm a rocket man
Rocket man burning out his fuse up here alone
And I think it's gonna be a long long time
Till touch down brings me round again to find
I'm not the man they think I am at home)";
  ASSERT_STREQ("SSdtIGEgcm9ja2V0IG1hbgpSb2NrZXQgbWFuIGJ1cm5pbmcgb3V0IGhpcyBmdXNlIHVwIGhlcmUgYWxvbmUKQW5kIEkgdGhpbmsgaXQncyBnb25uYSBiZSBhIGxvbmcgbG9uZyB0aW1lClRpbGwgdG91Y2ggZG93biBicmluZ3MgbWUgcm91bmQgYWdhaW4gdG8gZmluZApJJ20gbm90IHRoZSBtYW4gdGhleSB0aGluayBJIGFtIGF0IGhvbWU=", base64encode(buffer4, (size_t)strlen(buffer4)).c_str());

}
