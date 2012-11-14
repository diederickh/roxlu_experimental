#ifndef ROXLU_URL_PARSERH
#define ROXLU_URL_PARSERH

#include <string>
#include <algorithm>

struct URLInfo {
  std::string host;
  std::string filename;
  std::string file_ext;
  std::string proto;
  std::string subdomain;
  std::string path;
};

enum UPStates {
  UP_START
  ,UP_H
  ,UP_HT
  ,UP_HTT
  ,UP_HTTP
  ,UP_HTTPS
  ,UP_HTTP_C
  ,UP_HTTPS_C
  ,UP_HTTP_SLASH
  ,UP_SUBDOMAIN
  ,UP_HOST
  ,UP_FILE_EXT
  ,UP_FILE_NAME
  ,UP_PATH
  ,UP_EXIT
};
static bool parseURL(const char*  url, URLInfo& u) {
  int state = UP_START;
  int dx = 0;
  int size = strlen(url);
  while(dx < size) { 
    switch(state) {
      // h
    case UP_START: {
      if(url[dx] != 'h') {
        return  false;
      }
      u.proto.push_back(url[dx++]);
      state = UP_H;
      break;
    };
      // ht
    case UP_H:{
      if(url[dx] != 't') {
        return false;
      }
      u.proto.push_back(url[dx++]);
      state = UP_HT;
      break;
    }
      // htt
    case UP_HT: {
      if(url[dx] != 't') {
        return false;
      }
      u.proto.push_back(url[dx++]);
      state = UP_HTT;
      break;
    }

      // http
    case UP_HTT: {
      if(url[dx] != 'p') {
        return false;
      }
      u.proto.push_back(url[dx++]);
      state = UP_HTTP;
      break;
    }

      // https
    case UP_HTTP: {
      if(url[dx] == 's') {
        u.proto.push_back(url[dx++]);
        state = UP_HTTPS;
      }
      else if(url[dx] == ':') {
        state = UP_HTTP_C;
        dx++;
      }
      else {
        return false;
      }
      break;
    }
    case UP_HTTPS: {
      if(url[dx] == ':') {
        state = UP_HTTPS_C;
        dx++;
      }
      else {
        return false;
      }
      break;
    }
    case UP_HTTPS_C: {
      if(url[dx] == '/') {
        state = UP_HTTP_SLASH;
        dx++;
      }
      else {
        return false;
      }
      break;
    }
    case UP_HTTP_C: {
      if(url[dx] == '/') {
        state = UP_HTTP_SLASH;
        dx++;
      }
      else {
        return false;
      }
      break;
    }
    case UP_HTTP_SLASH: {
      if(url[dx] == '/') {
        state = UP_SUBDOMAIN;
        dx++;
      }
      else {
        return false;
      }
      break;
    }
    case UP_SUBDOMAIN: {
      while(dx < size) {
        if(url[dx] == '.') {
          state = UP_HOST;
          dx++;
          break;
        }
        else {
          u.subdomain.push_back(url[dx]);
        }
        dx++;
      }
      if(state != UP_HOST) {
        return false;
      }
      break;
    }
    case UP_HOST: {
      while(dx < size) {
        if(url[dx] == '.') {
          state = UP_FILE_EXT;
          dx++;
          break;
        }
        else {
          u.host.push_back(url[dx]);
        }
        dx++;
      }
      if(state != UP_FILE_EXT) {
        return false;
      }
      break;
    }

    case UP_FILE_EXT: {
      // now we go backwards
      int bdx = size-1;
      std::string part;
      while(bdx > dx) {
        if(state == UP_FILE_EXT && url[bdx] == '.') {
          u.file_ext = part;
          state = UP_FILE_NAME;
          bdx--;
          part.clear();
        }
        else if(state == UP_FILE_NAME && (url[bdx] == '/' || url[bdx] == '.')) {
          u.filename = part;
          state = UP_EXIT;
          bdx--;
          part.clear();
          break;
        }
        part.insert(part.begin(),url[bdx]);
        bdx--;
      }
      if(state != UP_EXIT) {
        return false;
      }
      return true;
      break;
    }

    };

  }
  return true;
}
#endif
