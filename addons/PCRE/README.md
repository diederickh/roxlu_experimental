# PCRE

This addon just makes sure that the correct libraries are added to the 
linker when you want to use the PCRE and PCRE-CPP library. Below are some 
examples of using this library.

_Extract two integers_
````c++
#include <pcre/PCRE.h>

int start, end;
std::string header = "Range: bytes=0-9999\r\n";
if(RE("Range: bytes=([0-9]+)-([0-9]+)").PartialMatch(header, &start, &end)) {
  RX_VERBOSE("MATCHED: %d, %d", start, end);
}
````

_Extract two integers, case insensitive_
````c++
int start, end;
std::string header = "range: bytes=0-9999\r\n";
if(RE("Range: bytes=([0-9]+)-([0-9]+)", RE_Options(PCRE_CASELESS)).PartialMatch(header, &start, &end)) {
  RX_VERBOSE("MATCHED: %d, %d", start, end);
}
````