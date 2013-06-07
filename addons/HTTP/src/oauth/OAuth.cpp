#include <oauth/OAuth.h>

OAuth::OAuth() {
  srand(time(NULL));
  updateNonceAndTimestamp();
}

OAuth::~OAuth() {
}

#define OAUTH_CONCAT_HEADER(str, n, sep)                      \
  {                                                           \
    if(params.exist(n)) {                                     \
      if(params[n].type == HTTP_PARAMETER_NAME_VALUE) {       \
        str += params[n].name +"=\"" +params[n].value +"\"";  \
        if(sep) {                                             \
          str +=", ";                                         \
        }                                                     \
      }                                                       \
    }                                                         \
  }

void OAuth::addAuthorizationHeadersToRequest(HTTPRequest& r) {
  std::string signature = createSignature(r);
  HTTPParameters params = collectParameters(r);
  
  params.add(HTTPParameter("oauth_signature", signature));
  params.percentEncode();

  std::string header = "OAuth ";

  OAUTH_CONCAT_HEADER(header, "oauth_callback", true);
  OAUTH_CONCAT_HEADER(header, "oauth_consumer_key", true);
  OAUTH_CONCAT_HEADER(header, "oauth_nonce", true);
  OAUTH_CONCAT_HEADER(header, "oauth_signature", true);
  OAUTH_CONCAT_HEADER(header, "oauth_signature_method", true);
  OAUTH_CONCAT_HEADER(header, "oauth_timestamp", true);
  OAUTH_CONCAT_HEADER(header, "oauth_token", true);
  OAUTH_CONCAT_HEADER(header, "oauth_version", false);

  r.addHeader(HTTPHeader("Authorization", header));
}

std::string OAuth::createSignature(HTTPRequest& r) {
  // Create signature base string and key
  PercentEncode encoder;
  std::string signature_base = createSignatureBaseString(r);
  std::string sign_key = createSignatureSignKey();

  // Create HMAC_SHA1 signature
  unsigned char digest[HMAC_SHA1_DIGEST_LENGTH];
  HMAC_SHA1_CTX ctx;
  HMAC_SHA1_Init(&ctx);
  HMAC_SHA1_UpdateKey(&ctx, (unsigned char*)sign_key.c_str(), sign_key.size());
  HMAC_SHA1_EndKey(&ctx);

  HMAC_SHA1_StartMessage(&ctx);
  HMAC_SHA1_UpdateMessage(&ctx, (unsigned char*)signature_base.c_str(), signature_base.size());
  HMAC_SHA1_EndMessage(digest, &ctx);

  // Base64 encode
  std::string result;
  aoBase64Encoder b64;
  b64.Encode((const unsigned char*)digest, HMAC_SHA1_DIGEST_LENGTH);
  int s = b64.GetEncodedSize();
  const char* encoded_data = b64.GetEncoded();
  std::copy(encoded_data, encoded_data+s, std::back_inserter(result));
  return result;
}

std::string OAuth::createSignatureBaseString(HTTPRequest& r) {
  HTTPURL url = r.getURL();
  std::string method = (r.getMethod() == HTTP_METHOD_POST) ? "POST" : "GET";

  // Get the percent encoded parameters
  HTTPParameters collected_parameters = collectParameters(r);
  collected_parameters.percentEncode();

  // We need to encode the parameter string (again)
  PercentEncode encoder;
  std::string parameter_string = collected_parameters.getQueryString();
  std::string normalized_url = createNormalizedURL(r.getURL());
  std::string signature_string = method +"&" +encoder.encode(normalized_url) +"&" +encoder.encode(parameter_string);

  return signature_string;
}

// create the normalized url string: 
// scheme://authority:port/path (see http://tools.ietf.org/html/rfc5849#section-3.4.1.2)
std::string OAuth::createNormalizedURL(HTTPURL& url) {
  std::string result;
  if(url.getProto() == HTTP_PROTO_HTTPS) {
    result = "https://";
  }
  else if(url.getProto() == HTTP_PROTO_HTTP) {
    result = "http://";
  }
  else {
    RX_ERROR("The HTTP proto (http/https) for the set url is not supported by oauth");
    ::exit(EXIT_FAILURE);
  }

  result += url.getHost();

  if(url.getPort() != "80" && url.getPort() != "443") {
    result += ":" +url.getPort();
  }

  result += url.getPath();

  return result;
}

HTTPParameters OAuth::collectParameters(HTTPRequest& r) {
  setParameters();

  HTTPParameters collected_parameters;
  HTTPParameters body_parameters = r.getContentParameters();
  HTTPParameters querystring_parameters = r.getURL().getQueryParameters();

  copyParametersForSignature(body_parameters, collected_parameters);
  copyParametersForSignature(querystring_parameters, collected_parameters);
  copyParametersForSignature(auth_params, collected_parameters);

  return collected_parameters;
}

// Not all parameters should be include int the collection that is used to create the signature base.
void OAuth::copyParametersForSignature(HTTPParameters& from, HTTPParameters& into) {
  into.copy(from, HTTP_PARAMETER_NAME_VALUE);
}

void OAuth::reset() {
  auth_params.clear();
  updateNonceAndTimestamp();
}

void OAuth::setParameters() {
  addNonceParameter();
  addTimestampParameter();
  addSignatureMethodParameter();
  addVersionParameter();
  addConsumerKeyParameter();
  addTokenParameter();
}

std::string OAuth::createSignatureSignKey() {
  PercentEncode encoder;
  std::string sign_key = encoder.encode(consumer_secret) +"&" +encoder.encode(token_secret);
  return sign_key;
}

void OAuth::addConsumerKeyParameter() {
  auth_params.add(HTTPParameter("oauth_consumer_key", consumer_key));
}

void OAuth::addNonceParameter() {
  auth_params.add(HTTPParameter("oauth_nonce",  nonce));
}

void OAuth::addTimestampParameter() {
  auth_params.add(HTTPParameter("oauth_timestamp", epoch));
}

void OAuth::addTokenParameter() {
  if(token.size()) {
    auth_params.add(HTTPParameter("oauth_token", token));
  }
}

void OAuth::addSignatureMethodParameter() {
  auth_params.add(HTTPParameter("oauth_signature_method", "HMAC-SHA1"));
}

void OAuth::addVersionParameter() {
  auth_params.add(HTTPParameter("oauth_version", "1.0"));
}

void OAuth::print() {
  printf("--\n");
  printf("OAUTH: Consumer key: %s\n", consumer_key.c_str());
  printf("OAUTH: Consumer secret: %s\n", consumer_secret.c_str());
  printf("OAUTH: Token: %s\n", token.c_str());
  printf("OAUTH: Signature method: HMAC-SHA\n"); 
  printf("OAUTH: Nonce: %s\n", nonce.c_str());
  printf("OAUTH: Timestamp: %llu\n", epoch);
  printf("--\n");
}

void OAuth::updateNonceAndTimestamp() {
  nonce = generateNonce();
  epoch = oauth_timestamp();
}

std::string OAuth::generateNonce() {
  const char alphanum[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  std::string result;
  for(int i = 0; i <= 16; ++i) {
    int dx = (char) (rand() % 62);
    char c  = alphanum[dx];
    result.push_back(c);
  }
  return result;
}

