#ifndef ROXLU_TWITTER_H
#define ROXLU_TWITTER_H

extern "C" {
#  include <uv.h>
}

#include <string>
#include <ssl/SSLContext.h>
#include <oauth/OAuth.h>
#include <http/HTTP.h>
#include <http/HTTPConnection.h>
#include <twitter/TwitterTypes.h>
#include <twitter/Tweet.h>

class Twitter {

 public:
  Twitter();
  ~Twitter();
  bool setup(std::string token, std::string tokenSecret, std::string consumer, std::string consumerSecret);             /* setup the twitter api lib. token/tokenSecret represent the keys of the user upon which we will perform requests. the consumer/con.Secret are used to identitify your applications. the ssl key is the ssl private key that we use to encrypte the http requests  */
  void update();
  void apiStatusesFilter(TwitterStatusesFilter& param, httpconnection_event_callback cbEvent, void* user);
  void apiStatusesUpdate(TwitterStatusesUpdate& param, httpconnection_event_callback cbEvent, void* user);
  void apiStatusesUpdateWithMedia(TwitterStatusesUpdate& param, httpconnection_event_callback cbEvent, void* user);

 private:
  void setDefaultParams(TwitterCallParams& p);
  void makeAPICall(TwitterCallParams& p);                                                                               /* helper that creates an request object and makes sure the request is made..*/

 private:
  SSLContext ssl_ctx;                                                                                                   /* the SSL context we use to allocate SSL objects for encrypting requests */
  OAuth oauth;                                                                                                          /* the requests must be signed using oauth */
  HTTP http;                                                                                                            /* our http context that we used to perform all the http requests */
};

#endif
