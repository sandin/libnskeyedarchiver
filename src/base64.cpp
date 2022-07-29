#include "nskeyedarchiver/base64.hpp"

static const char base64_chars[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789"
    "+/=";
static const char base64_pad = '=';

// Base64 RFC: https://datatracker.ietf.org/doc/html/rfc3548
// Inspired by: https://github.com/google/skia/blob/main/src/utils/SkBase64.cpp
std::string nskeyedarchiver::base64encode(const char* buffer, size_t size) {
  std::string encoded;
  encoded.reserve((size + 2) / 3 * 4);
  
  int i = 0;
  size_t remainder = size % 3;
  size_t safe_end = size - (remainder);
  while (i < safe_end) {
    unsigned char a = buffer[i++];
    unsigned char b = buffer[i++];
    unsigned char c = buffer[i++];
    // a        b        c
    // 00000000 00000000 00000000
    // |  a ||  b  ||  c  || d  |
    int d = c & 0b00111111;
    c = (c >> 6 | b << 2) & 0b00111111;
    b = (b >> 4 | a << 4) & 0b00111111;
    encoded.push_back(base64_chars[a >> 2]);
    encoded.push_back(base64_chars[b]);
    encoded.push_back(base64_chars[c]);
    encoded.push_back(base64_chars[d]);
  }
  if (remainder > 0) {
    int a = (uint8_t)buffer[i++];
    int b = 0;
    int c = 64;
    if (remainder == 2) {
      int b1 = buffer[i++];
      b = (b1 >> 4 | a << 4) & 0b00111111;
      c = (b1 << 2) & 0b00111111;
    } else { // remainder == 1
      b = (b | a << 4) & 0b00111111;
    }
    encoded.push_back(base64_chars[a >> 2]);
    encoded.push_back(base64_chars[b]);
    encoded.push_back(base64_chars[c]);
    encoded.push_back(base64_pad);
  }
  return encoded;
}
