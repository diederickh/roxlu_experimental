#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <instagram/Instagram.h>
#include <http_parser.h>

using namespace roxlu;
http_parser_settings parser_settings;
http_parser parser;
size_t total;

static int on_parser_simple(http_parser *p) { return 0; }
static int on_parser_data(http_parser* p, const char* at, size_t len) { return 0; }
static void on_subscription_list(const char* data, size_t len, void* user) { }
static void on_locations_search(const char* data, size_t len, void* user) { }

// --------------------------------------------------
// PARSING CHUNKED HTTP RESPINSE
// --------------------------------------------------
static int on_popular_media_body(http_parser* p, const char* data, size_t len) {
  total += len;
  std::string str(data, len);
  printf("%s\n", str.c_str());
  return 0;
}

// Gets called when we the server disconnects us, data contains the full response
static void on_popular_media(const char* data, size_t len, void* user) {
  std::ofstream ofs("response.raw");
  if(ofs.is_open()) {
    ofs.write((char*)data, len);
    ofs.close();
  }
  int n = http_parser_execute(&parser, &parser_settings, data, len);
}

static void on_media_search(const char* data, size_t len, void* user) { 
  int n = http_parser_execute(&parser, &parser_settings, data, len);
}

int main() {
  total = 0;
  memset(&parser_settings, 0, sizeof(parser_settings));
  parser_settings.on_message_begin = on_parser_simple;
  parser_settings.on_url = on_parser_data;
  parser_settings.on_header_field = on_parser_data;
  parser_settings.on_header_value = on_parser_data;
  parser_settings.on_headers_complete = on_parser_simple;
  parser_settings.on_message_complete = on_parser_simple;
  parser_settings.on_body = on_popular_media_body;
  http_parser_init(&parser, HTTP_RESPONSE);

  Instagram ins; 
  ins.setup(
            "e3a93c4ff4a24f1ebe4580736904c545", 
            "bfbe3df9b2ae4741947f0e922bec3131", 
            "/Users/diederickhuijbers/roxlu/tests/instagram/bin/client-key.pem"
            );
  ins.setAccessToken("29281621.e3a93c4.472bb7156cd64bb4ad069fb5396919e1");
  //ins.mediaPopular(NULL,on_popular_media, &ins);

  // search for images around this point.
  HTTPParams search_params;
  search_params.add("lat", "52.153377");
  search_params.add("lng", "4.509437");
  ins.mediaSearch(search_params, NULL, on_media_search, &ins);

  while(true) {
    ins.update();
  }
}
