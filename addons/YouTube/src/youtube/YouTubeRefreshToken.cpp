#include <youtube/YouTubeRefreshToken.h>
#include <roxlu/core/Log.h>
#include <roxlu/core/Utils.h>


size_t youtube_refresh_token_data_cb(char* ptr, size_t size, size_t nmemb, void* user) {
  size_t num_bytes = size * nmemb;

#if 0
    printf("--------------------------------------------\n");
    for(size_t i = 0; i < num_bytes; ++i) {
      printf("%c", ptr[i]);
    }
    printf("+++++++++++++++++++++++++++++++++++++++++++++\n");
#endif

  YouTubeRefreshToken* t = static_cast<YouTubeRefreshToken*>(user);
  t->json.append(ptr, 0, num_bytes);
  return num_bytes;
}

// -----------------------------------------------------------------------------------

YouTubeRefreshToken::YouTubeRefreshToken()
  :curl(NULL)
  ,expire_value(0)
{
  curl_global_init(CURL_GLOBAL_DEFAULT);
}

YouTubeRefreshToken::~YouTubeRefreshToken() {
  curl = NULL;
  json = "";
  expire_value = 0;
}

bool YouTubeRefreshToken::refresh(std::string clientID, std::string clientSecret, std::string refreshToken) {
  CURLcode res;
  struct curl_httppost* form_post = NULL;
  struct curl_httppost* last_ptr = NULL;

  if(curl) {
    RX_ERROR("Already refreshing.. ");
    return false;
  }

  curl = curl_easy_init();
  if(!curl) {
    RX_ERROR("Cannot initialize curl easy handle");
    return false;
  }


  // add options
  res = curl_easy_setopt(curl, CURLOPT_URL, "https://accounts.google.com/o/oauth2/token");
  YT_CURL_ERR(res);

  res = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
  YT_CURL_ERR(res);

  res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, youtube_refresh_token_data_cb);
  YT_CURL_ERR(res);

  res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
  YT_CURL_ERR(res);

  res = curl_easy_setopt(curl, CURLOPT_POST, 1L);
  YT_CURL_ERR(res);

  res = curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);                                      
  YT_CURL_ERR(res);


  // form
  // @todo error check
  curl_formadd(&form_post, &last_ptr, CURLFORM_COPYNAME, "client_id", CURLFORM_COPYCONTENTS, clientID.c_str(), CURLFORM_END);
  curl_formadd(&form_post, &last_ptr, CURLFORM_COPYNAME, "client_secret", CURLFORM_COPYCONTENTS, clientSecret.c_str(), CURLFORM_END);
  curl_formadd(&form_post, &last_ptr, CURLFORM_COPYNAME, "refresh_token", CURLFORM_COPYCONTENTS, refreshToken.c_str(), CURLFORM_END);
  curl_formadd(&form_post, &last_ptr, CURLFORM_COPYNAME, "grant_type", CURLFORM_COPYCONTENTS, "refresh_token", CURLFORM_END);

  res = curl_easy_setopt(curl, CURLOPT_HTTPPOST, form_post);
  YT_CURL_ERR(res);
  
  // request
  res = curl_easy_perform(curl);
  YT_CURL_ERR(res);

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

bool YouTubeRefreshToken::parseJSON() {
  if(!json.size()) {
    RX_ERROR("the json member is empty");
    return false;
  }

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

  // get expires values
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
  RX_VERBOSE("New access token: `%s`", access_token.c_str());
  json_decref(root);
  root = NULL;
  return true;
}
