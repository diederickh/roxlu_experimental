#include <crypto/PercentEncode.h>

PercentEncode::PercentEncode() {
  for(int i = 0; i < 256; ++i) {
    rfc3986[i] = isalnum(i) || i == '~'||i == '-'||i == '.'|| i == '_'  ? i : 0;
  }
}

std::string PercentEncode::encode(std::string input) {
  std::string result;
  char buf[5] = {};
  char encoded[2] = {};
  for(int i = 0; i < input.size(); ++i) {
    char c = input[i];
    if(rfc3986[c]) {
      sprintf(encoded, "%c", rfc3986[c]);
      result.push_back(encoded[0]);
    }
    else {
      sprintf(buf, "%%%02X", c);
      for(int j = 0; j < strlen(buf); ++j) {
        result.append(&buf[j], 1);
      }
    }
  }
  return result;
}

