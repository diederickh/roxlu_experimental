#include <twitter/Twitter.h>
#include <iostream>
//#include <unistd.h>
#include <roxlu/Roxlu.h>

// Our callbacks
void on_access_token_received_callback(roxlu::twitter::TwitterTokenInfo info, void* data);
void on_request_token_callback(std::string token, std::string tokenSecret, std::string redirectURL, void* data);

int main() {
  // On linux + mac, get the filepath to the SSL client key (see twitter addon readme on how to create one)
  /*
  char dir[1024];
  if(getcwd(dir, sizeof(dir)) == NULL) {
    printf("ERROR: Cannot get current directory.\n");
    ::exit(0);
  }
  char key_file[1024];
  sprintf(key_file, "%s/%s", dir, "data/client-key.pem");
  */
  //  std::string key_file = rx_to_data_path("client-key.pem");

  // Get a request-token, then exchange this token for a aouth token (see callback);
  roxlu::twitter::Twitter tw;
  tw.setConsumer("q8mQUYq7AEjGX4qD8lxSKw"); // Get this from your apps settings
  tw.setConsumerSecret("Uy3toRg4OXZqwuNTk9HYTaQLX977DenO0FG8rT5v6A"); // Get this from your apps settings
  tw.requestToken(on_request_token_callback, &tw);
  
  while(true) {
    tw.update();
  }
  return 0;
};


// When we received the request-token, we can exchange this for a real token.
void on_request_token_callback(std::string token, std::string tokenSecret, std::string redirectURL, void* data) {
  roxlu::twitter::Twitter* t = static_cast<roxlu::twitter::Twitter*>(data);
  printf("Request token: %s, token_secret: %s\n", token.c_str(), tokenSecret.c_str());
  std::string pin;
  printf("Open this URL: %s\npin:", redirectURL.c_str());
  std::cin >> pin;
  printf("Entered:%s\n", pin.c_str());
  t->exchangeRequestTokenForAccessToken(token, pin, on_access_token_received_callback, t);
  printf("called..\n");
}

// This is called when we exchanged the request-token for access token. Make sure to save this values
// somewhere on disk and use them for 'setToken()' and 'setTokenSecret'
void on_access_token_received_callback(roxlu::twitter::TwitterTokenInfo info, void* data) {
  printf("Store the following token and secret somewhere safe:\n");
  printf("\n+++++++++++++++++++++++++++++++++++++++++++++++++\n");
  printf("OAuth token: %s\n", info.oauth_token.c_str());
  printf("OAuth token secret: %s\n", info.oauth_token_secret.c_str());
  printf("User id: %s\n", info.user_id.c_str());
  printf("User name: %s\n", info.screen_name.c_str());
  printf("+++++++++++++++++++++++++++++++++++++++++++++++++\n");
}
