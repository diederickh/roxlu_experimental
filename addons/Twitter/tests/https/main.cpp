#include <iostream>
#include <twitter/Twitter.h>
#include <libgen.h>

void on_status_update_callback(const char* data, size_t len, void* user);
void on_filter_update_callback(const char* data, size_t len, void* user);
void on_access_token_received_callback(TwitterTokenInfo info, void* data);
void on_request_token_callback(std::string token, std::string tokenSecret, std::string redirectURL, void* data);

int main() {
 char client_key_file[1024];
 sprintf(client_key_file, "%s/%s", dirname(__FILE__), "client-key.pem");

  Twitter tw;
  tw.setSSLPrivateKey(client_key_file);
  tw.setConsumer("q8mQUYq7AEjGX4qD8lxSKw"); // Get this from your apps settings
  tw.setConsumerSecret("Uy3toRg4OXZqwuNTk9HYTaQLX977DenO0FG8rT5v6A"); // Get this from your apps settings
  tw.setToken("466622389-7ksRhjypPoUDo5DTkH3tIp2m9zhgxDVsIAFOkKw");
  tw.setTokenSecret("xnhq6bW19DuRPUhMswXddn99v1BhefX3ePwstz5nqc");

  TwitterStatusesFilter tsf("twitter,love,sex,friend");
  tw.apiStatusesFilter(tsf, on_status_update_callback); 

  while(true) {
    tw.update();
  }
  return 0;
}

void on_status_update_callback(const char* data, size_t len, void* user) {
  printf("\n============================================\n");
  std::copy(data, data+len, std::ostream_iterator<char>(std::cout, ""));
  printf("\n============================================\n");
}

void on_filter_update_callback(const char* data, size_t len, void* user) {
  printf("\n+++++++++++++++++++++++++++++++++++++++++++++++++\n");
  std::copy(data, data+len, std::ostream_iterator<char>(std::cout, ""));
  printf("\n+++++++++++++++++++++++++++++++++++++++++++++++++\n");
}

void on_access_token_received_callback(TwitterTokenInfo info, void* data) {
  printf("Store the following token and secret somewhere safe:\n");
  printf("\n+++++++++++++++++++++++++++++++++++++++++++++++++\n");
  printf("OAuth token: %s\n", info.oauth_token.c_str());
  printf("OAuth token secret: %s\n", info.oauth_token_secret.c_str());
  printf("User id: %s\n", info.user_id.c_str());
  printf("User name: %s\n", info.screen_name.c_str());
  printf("+++++++++++++++++++++++++++++++++++++++++++++++++\n");
}

void on_request_token_callback(std::string token, std::string tokenSecret, std::string redirectURL, void* data) {
  Twitter* t = static_cast<Twitter*>(data);
  printf("Request token: %s, token_secret: %s\n", token.c_str(), tokenSecret.c_str());
  std::string pin;
  printf("Open this URL: %s\npin:", redirectURL.c_str());
  std::cin >> pin;
  printf("Entered:%s\n", pin.c_str());
  t->exchangeRequestTokenForAccessToken(token, pin, on_access_token_received_callback, t);
  printf("called..\n");
}
