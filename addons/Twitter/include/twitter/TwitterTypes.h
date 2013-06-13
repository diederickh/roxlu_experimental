#ifndef ROXLU_TWITTER_TYPES_H
#define ROXLU_TWITTER_TYPES_H

#include <string>
#include <http/HTTPConnection.h>
#include <http/HTTPTypes.h>
#include <http/HTTPParameters.h>

//-------------------------------------------------------------------------------

// Generic wrapper for making calls to the twitter servers
class TwitterCallParams {
 public:
  TwitterCallParams();
  bool addFile(std::string name, std::string filename, bool datapath = false);
  void setEndPoint(std::string endp);
  void setHost(std::string host);
  void setIsSecure(bool flag);
  void setMethod(HTTPMethod m);
  void setCallback(httpconnection_event_callback dataCB, void* user);

 public:
  std::string endpoint;
  std::string host;
  HTTPMethod method;
  HTTPParameters content_parameters;
  HTTPParameters query_parameters;
  bool is_secure;
  void* cb_user;
  httpconnection_event_callback cb_response;
};

//-------------------------------------------------------------------------------

inline void TwitterCallParams::setEndPoint(std::string endp) {
  endpoint = endp;
}

inline void TwitterCallParams::setHost(std::string h) {
  host = h;
}

inline void TwitterCallParams::setIsSecure(bool flag) {
  is_secure = flag;
}

inline void TwitterCallParams::setMethod(HTTPMethod m) {
  method = m;
}

inline void TwitterCallParams::setCallback(httpconnection_event_callback dataCB, void* user) {
  cb_user = user;
  cb_response = dataCB;
}

//-------------------------------------------------------------------------------

// used when doing a status update
class TwitterStatusesUpdate : public TwitterCallParams {
 public:
  TwitterStatusesUpdate(std::string status);
  bool addMedia(std::string filename, bool datapath = false);                        /* add media to the post; returns false if the file wasn't found */

 public:
  std::string status;
};

//-------------------------------------------------------------------------------
class TwitterStatusesFilter : public TwitterCallParams {
 public:
  void track(std::string trackString);
  void follow(std::string followString);
};

#endif
