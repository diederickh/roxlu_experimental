#ifndef ROXLU_PERCENT_ENCODEH
#define ROXLU_PERCENT_ENCODEH

#include <iostream>
#include <string>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

class PercentEncode {
 public:
  PercentEncode();
  std::string encode(std::string input);
 private:
  char rfc3986[256];
};

#endif
