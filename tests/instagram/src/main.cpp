#include <stdio.h>
#include <iostream>
#include <fstream>
#include <instagram/Instagram.h>
#include <http_parser.h>

using namespace roxlu;
http_parser_settings parser_settings;
http_parser parser;
size_t total;

static int on_parser_simple(http_parser *p) {
  return 0;
}

static int on_parser_data(http_parser* p, const char* at, size_t len) {
  return 0;
}


static void on_subscription_list(const char* data, size_t len, void* user) {
}

static int on_popular_media_body(http_parser* p, const char* data, size_t len) {
  total += len;
  //printf("> %zu, %zu\n", len, total);
  //printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n%s\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n", data);
  return 0;
}

static void on_popular_media(const char* data, size_t len, void* user) {
   printf("RECEIVED: %zu\n", len);
  std::ofstream ofs("response.raw");
  if(ofs.is_open()) {
    ofs.write((char*)data, len);
    ofs.close();
  }
  int n = http_parser_execute(&parser, &parser_settings, data, len);
}


static void on_locations_search(const char* data, size_t len, void* user) {
  printf("----------\n%s\n----------------\n", data);
}

static void on_media_search(const char* data, size_t len, void* user) {
 printf("----------\n%s\n----------------\n", data);
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
  ins.setup("e3a93c4ff4a24f1ebe4580736904c545", "bfbe3df9b2ae4741947f0e922bec3131", "/Users/diederickhuijbers/roxlu/tests/instagram/bin/client-key.pem");
  ins.setAccessToken("29281621.e3a93c4.472bb7156cd64bb4ad069fb5396919e1");
  //ins.mediaPopular(NULL,on_popular_media, &ins);

  HTTPParams p;
  p.add("lat","52.162455");
  p.add("lng", "4.494756");
  // ins.locationsSearch(p, on_locations_search, NULL, &ins);
 ins.mediaSearch(p, on_media_search, NULL, &ins);
  //    void Instagram::locationsSearch(HTTPParams& params, cb_instagram_on_data dataCB, cb_instagram_on_close closeCB, void* user) {
  while(true) {
    ins.update();
  }
}
