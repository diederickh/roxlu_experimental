#include <twitter/OAuth.h>
#include <twitter/Types.h>

OAuth::OAuth() {
  srand(time(NULL));
  updateNonce();
}

OAuth::~OAuth() {
}

#define CONCAT_HEADER(str, n, sep) \
if(params.exist(n)) { \
    str += params[n].name +"=\"" +params[n].value +"\""; \
    if(sep) { \
      str +=", "; \
    } \
 }

void OAuth::addAuthorizationHeadersToRequest(HTTPRequest& r) {
  std::string signature = createSignature(r);
  HTTPParameters params = collectParameters(r);
  params.add(HTTPParameter("oauth_signature", signature));
  params.percentEncode();

  std::string header = "OAuth ";
  CONCAT_HEADER(header, "oauth_callback", true);
  CONCAT_HEADER(header, "oauth_consumer_key", true);
  CONCAT_HEADER(header, "oauth_nonce", true);
  CONCAT_HEADER(header, "oauth_signature", true);
  CONCAT_HEADER(header, "oauth_signature_method", true);
  CONCAT_HEADER(header, "oauth_timestamp", true);
  CONCAT_HEADER(header, "oauth_token", true);
  CONCAT_HEADER(header, "oauth_version", false);

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
  std::string method = (r.getMethod() == REQUEST_POST) ? "POST" : "GET";

  // Get the percent encoded parameters
  HTTPParameters collected_parameters = collectParameters(r);
  collected_parameters.percentEncode();

  // We need to encode the parameter string (again)
  PercentEncode encoder;
  std::string parameter_string = collected_parameters.getQueryString();
  std::string signature_string = method +"&" +encoder.encode(url.getString()) +"&" +encoder.encode(parameter_string);
  return signature_string;
}

HTTPParameters OAuth::collectParameters(HTTPRequest& r) {
  setParameters();

  HTTPParameters collected_parameters;
  HTTPParameters body_parameters = r.getContentParameters();
  HTTPParameters querystring_parameters = r.getQueryStringParameters();
  collected_parameters.copy(body_parameters);
  collected_parameters.copy(querystring_parameters);
  collected_parameters.copy(auth_params);
  return collected_parameters;
}

void OAuth::reset() {
  callback.clear();
  auth_params.clear();
}

void OAuth::setParameters() {
  addNonceParameter();
  addTimestampParameter();
  addSignatureMethodParameter();
  addVersionParameter();
  addConsumerKeyParameter();
  addTokenParameter();
  addCallbackParameter();
}

std::string OAuth::createSignatureSignKey() {
  PercentEncode encoder;
  //  std::string sign_key = encoder.encode(consumer_secret) +"&" +encoder.encode(token);
  std::string sign_key = encoder.encode(consumer_secret) +"&" +encoder.encode(token_secret);
  return sign_key;
}

void OAuth::addConsumerKeyParameter() {
  auth_params.add("oauth_consumer_key", consumer_key);
}

void OAuth::addNonceParameter() {
  auth_params.add("oauth_nonce",  nonce);
}

void OAuth::addTimestampParameter() {
  auth_params.add("oauth_timestamp", epoch);
}

void OAuth::addTokenParameter() {
  if(token.size()) {
    auth_params.add("oauth_token", token);
  }
}

void OAuth::addSignatureMethodParameter() {
  auth_params.add("oauth_signature_method", "HMAC-SHA1");
}

void OAuth::addVersionParameter() {
  auth_params.add("oauth_version", "1.0");
}

void OAuth::addCallbackParameter() {
  if(callback.size()) {
    auth_params.add("oauth_callback", callback);
  }
}

void OAuth::updateNonce() {
  nonce = makeNonce();
  epoch = timestamp();
}

std::string OAuth::makeNonce() {
  const char alphanum[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  std::string nonce;
  for(int i = 0; i <= 16; ++i) {
    int dx = (rand() % sizeof(alphanum)-1);
    nonce += alphanum[dx];
  }
  return nonce;
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
