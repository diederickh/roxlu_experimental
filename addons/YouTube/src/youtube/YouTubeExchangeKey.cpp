#include <youtube/YouTubeExchangeKey.h>
#include <roxlu/core/Log.h>
#include <roxlu/core/Utils.h>


size_t youtube_exchange_key_data_cb(char* ptr, size_t size, size_t nmemb, void* user) {
  size_t num_bytes = size * nmemb;

#if 0
  printf("--------------------------------------------\n");
  for(size_t i = 0; i < num_bytes; ++i) {
    printf("%c", ptr[i]);
  }
  printf("+++++++++++++++++++++++++++++++++++++++++++++\n");
#endif

  YouTubeExchangeKey* k = static_cast<YouTubeExchangeKey*>(user);
  k->json.append(ptr, 0, num_bytes);
  return num_bytes;
}

// -----------------------------------------------------------------------------------

YouTubeExchangeKey::YouTubeExchangeKey() 
  :curl(NULL)
  ,expire_value(0)
{
  curl_global_init(CURL_GLOBAL_DEFAULT);
}

YouTubeExchangeKey::~YouTubeExchangeKey() {

  if(curl) {
    curl_easy_cleanup(curl);
    RX_ERROR("We shouldn't arrive here...(!?)");
  }

  curl = NULL;
  json = "";
  access_token = "";
  refresh_token = "";  
  expire_value = 0;
}

bool YouTubeExchangeKey::exchange(std::string code, std::string clientID, std::string clientSecret) {
  CURLcode res;
  struct curl_httppost* form_post = NULL;
  struct curl_httppost* last_ptr = NULL;
  long http_code = 0;

  if(curl) {
    RX_ERROR("Already exchanging a key...");
    return false;
  }

  if(!code.size()) {
    RX_ERROR("Invalid code");
    return false;
  }

  curl = curl_easy_init();
  if(!curl) {
    RX_ERROR("Cannot initialize curl easy handle");
    return false;
  }

  // general options
  res = curl_easy_setopt(curl, CURLOPT_URL, "https://accounts.google.com/o/oauth2/token");
  YT_CURL_ERR(res);

  res = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
  YT_CURL_ERR(res);

  res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, youtube_exchange_key_data_cb);
  YT_CURL_ERR(res);

  res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
  YT_CURL_ERR(res);

  res = curl_easy_setopt(curl, CURLOPT_POST, 1L);
  YT_CURL_ERR(res);

  res = curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);                                      
  YT_CURL_ERR(res);

  RX_VERBOSE("CODE: %s", code.c_str());
  RX_VERBOSE("CLIENT_ID: %s", clientID.c_str());
  RX_VERBOSE("CLIENT_SECRET: %s", clientSecret.c_str());
  // form 
  // @todo error check
  curl_formadd(&form_post, &last_ptr, CURLFORM_COPYNAME, "code", CURLFORM_COPYCONTENTS, code.c_str(), CURLFORM_END);
  curl_formadd(&form_post, &last_ptr, CURLFORM_COPYNAME, "client_id", CURLFORM_COPYCONTENTS, clientID.c_str(), CURLFORM_END);
  curl_formadd(&form_post, &last_ptr, CURLFORM_COPYNAME, "client_secret", CURLFORM_COPYCONTENTS, clientSecret.c_str(), CURLFORM_END);
  curl_formadd(&form_post, &last_ptr, CURLFORM_COPYNAME, "redirect_uri", CURLFORM_COPYCONTENTS, "urn:ietf:wg:oauth:2.0:oob", CURLFORM_END);
  curl_formadd(&form_post, &last_ptr, CURLFORM_COPYNAME, "grant_type", CURLFORM_COPYCONTENTS, "authorization_code", CURLFORM_END);

  res = curl_easy_setopt(curl, CURLOPT_HTTPPOST, form_post);
  YT_CURL_ERR(res);

  // request
  res = curl_easy_perform(curl);
  YT_CURL_ERR(res);

  // check result
  res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
  YT_CURL_ERR(res);
  if(http_code != 200) {
    RX_ERROR("HTTP result was not successfull while trying to exchange the auth key, got: %ld", http_code);
    if(http_code == 400) {
      RX_ERROR("This means that the authorization key you provided is not valid anymore; renew it using the youtube/html/index.html, see addon documentation");
    }
    else if(http_code == 401) {
      RX_ERROR("The given authorization key has expired. Open the youtube/html/index.html, and get a new one. Remove the database too;");
    }
    goto error;
  }

  // cleanup
  curl_easy_cleanup(curl);
  curl_formfree(form_post);
  curl = NULL;
  form_post = NULL;
  last_ptr = NULL;

  if(!parseJSON()) {
    goto error;
  }

  return true;

 error:
  if(curl) {
    curl_easy_cleanup(curl);
    curl = NULL;
  }

  if(form_post) {
    curl_formfree(form_post);
    form_post = NULL;
    last_ptr = NULL;
  }

  return false;
}

bool YouTubeExchangeKey::parseJSON() {

  // validate
  if(!json.size()) {
    RX_ERROR("The json member is empty. Cannot exchange key.");
    return false;
  }

  // parse json
  json_t* root = NULL;
  json_error_t err;

  root = json_loads(json.c_str(), 0, &err);

  if(!root) {
    RX_ERROR("Error with json, line: %d, message: %s", err.line, err.text);
    json_decref(root);
    root = NULL;
    return false;
  }

  // get access token
  json_t* atoken = json_object_get(root, "access_token");
  if(!json_is_string(atoken)) {
    RX_ERROR("Cannot find access_token in result from YouTube");
    json_decref(root);
    root = NULL;
    return false;
  }

  std::string atoken_str = json_string_value(atoken);
  if(!atoken_str.size()) {
    RX_ERROR("Invalid access token; empty");
    json_decref(root);
    root = NULL;
    return false;
  }
      
  // refresh token
  json_t* rtoken = json_object_get(root, "refresh_token");
  if(!json_is_string(rtoken)) {
    RX_ERROR("Cannot find refresh_token in result from YouTube");
    json_decref(root);
    root = NULL;
    return false;
  }

  std::string rtoken_str = json_string_value(rtoken);
  if(!rtoken_str.size()) {
    RX_ERROR("Invalid refresh token; empty");
    json_decref(root);
    root = NULL;
    return false;
  }
     
  //  expire timeout
  json_t* expire = json_object_get(root, "expires_in");
  if(!json_is_integer(expire)) {
    RX_ERROR("Cannot find the expires_in in result form YouTube");
    json_decref(root);
    root = NULL;
    return false;
  }

  expire_value =  json_number_value(expire);
  if(!expire || expire < 0) {
    RX_ERROR("Invalid expire value: %d", expire_value);
    json_decref(root);
    root = NULL;
    return false;
  }

  expire_value = rx_time() + expire_value;
  access_token = atoken_str;
  refresh_token = rtoken_str;
      
  json_decref(root);
  root = NULL;

  return true;
}
