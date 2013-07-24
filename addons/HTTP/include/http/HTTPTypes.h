#ifndef ROXLU_HTTP_TYPES_H
#define ROXLU_HTTP_TYPES_H

#include <string>

enum HTTPMethod {
  HTTP_METHOD_GET,
  HTTP_METHOD_POST,
  HTTP_METHOD_PUT
};

enum HTTPVersion {
  HTTP_VERSION_1_0,
  HTTP_VERSION_1_1
};

enum HTTPParameterType {
  HTTP_PARAMETER_NAME_VALUE,
  HTTP_PARAMETER_FILE
};

enum HTTPProto {
  HTTP_PROTO_HTTP,
  HTTP_PROTO_HTTPS
};

enum HTTPHeaderType {
  HTTP_HEADER_NAME_VALUE,
  HTTP_HEADER_VALUE
};

enum HTTPFormEncoding {
  HTTP_FORM_ENCODING_NONE,
  HTTP_FORM_URL_ENCODED,
  HTTP_FORM_MULTIPART
};

// events which are dispatched by a HTTPConnection
enum HTTPConnectionEvent {
  HTTP_ON_CLOSED,
  HTTP_ON_BODY,                                                   /* notifies when new response data has been parsed, data is passed into the handler */
  HTTP_ON_STATUS,                                                 /* we got a http status code, see HTTPConnection->parser.status_code, data = NULL, len = 0 */
  HTTP_ON_HEADERS,                                                /* when we parsed all the response headers, data =  NULL, len = 0 */
  HTTP_ON_COMPLETE,                                               /* when parsing has completed */
};

// Used by the HTTPParameter 
struct HTTPFile {
  HTTPFile();
  HTTPFile(std::string filename, bool datapath = false);         
  bool setFile(std::string filename, bool datapath = false);     /* set the file */
  bool exists();                                                 /* check if the file exists */
  void setContentType(std::string ct);
  void setTransferEncoding(std::string te);
  void setDefaults();                                            /* set defaults (content_type, transfer encoding etc..) */

  std::string filepath;
  std::string filename;
  std::string content_type;                                      /* defaults to `application/octet-stream` */
  std::string transfer_encoding;                                 /* defaults to `binary` */
};

inline void HTTPFile::setContentType(std::string ct) {
  content_type = ct;
}

inline void HTTPFile::setTransferEncoding(std::string te) {
  transfer_encoding = te;
}

#endif
