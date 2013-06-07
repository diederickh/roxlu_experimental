# Twitter

This library is a minimal C++ library for the Twitter REST and Streaming
API. It's written in pure C++/C and uses [openSSL](http://www.openssl.org/), the excellent [libuv](https://github.com/joyent/libuv)
library for networking and some third party algorithms for HMAC-SHA1 (see HTTP addon).

## How to use this API
If you really want to understand how everything works you need to read up on [oauth](http://www.oauth.net),
[openSSL](http://www.openssl.org) and [libuv](https://github.com/joyent/libuv). For this addon it's important
to know that twitter has a couple of different [API categories](https://dev.twitter.com/start).

#### 1. Create an application

You need to create an application at the [developers site](http://developer.twitter.com) of Twitter. You need
to login and then goto to [My Applications](https://dev.twitter.com/apps) where you can create a new 
application by clicking on the **Create new application** button. Your application will use oauth to make
requests on behalf of the user who authorizes the application. If you're like me, and mainly work on 
[interactive installations](http://www.apollomedia.nl) you will create a new application and a new
user for the installation and authorize your application with the user.  At this point it's probably good
to read up on [oauth](http://www.oauth.net), but if you're in a hurry it's good to know these terms:

 - consumer key + secret: These values represent your application 
 - access token + secret: These values represent the user who granted access to the application

#### 2. Setup the Twitter object

Once you've created the application, Twitter gives you all the necessary values for the consumer and
user tokens. Go to your application setting, and see the **OAuth settings**. The following examples shows
how to setup the `Twitter` object.

````c++

   bool r =  twitter.setup("466622389-...",   /* the token */
                           "eH25IAxRIB...",   /* the token secret */
                           "e0vURm6xh....",   /* the consumer key */ 
                           "R7HfL0vgy2F....");/* the consumer secret */
   if(!r) {
     RX_ERROR("Cannot setup the twitter obj");
     ::exit(EXIT_FAILURE);
   }

````

Make sure to call `twitter.update()` as often as possible. We're using non-blocking sockets, so 
the socket backend needs check if there is data to be processed on a regularly basis. So e.g. use:

````c++
void testApp::update() {
     twitter.update();
}
````

#### 3. Making API requests

All api functions start with the prefix _api_ and I tried to give each function the same name
as the api endpoint. For example, the api call [statuses/update_with_media](https://dev.twitter.com/docs/api/1/post/statuses/update_with_media)
can be accessed using `twitter.apiStatusesUpdateWithMedia`. All the api functions follow the same scheme:
`functionName(parameterObject, callback_function, callback_data)`. The `parameterObject` is an object
on which you set the parameters for the api call. For the call `apiStatusesUpdateWithMedia` you pass an object
of the type `TwitterStatusesUpdate`. For information on how to use the parameter objects, see the `TwitterTypes.h`
file or the [twitter examples](https://github.com/roxlu/roxlu/tree/master/apps/examples). The next parameter is a
callback function that will be called when requests finishes or when we receive data from Twitter. All callbacks
must follow this definition:

````c++
void twitter_callback(HTTPConnection* c, HTTPConnectionEvent event, const char* data, size_t len, void* user)
````

The `HTTPConnection` is a pointer to the underlying socket connection and is used to connect and transfer 
data to the Twitter servers. `HTTPConnectionEvent` is an integer value and represents the type of event for
which the callback got called. When we've parsed the http response headers and know the http status, the 
`HTTPConnectionEvent` will have a value like `HTTP_ON_STATUS`, when we read data from the body section it
will have an value of `HTTP_ON_BODY`. Use this event value to execute the appropriate code. `data` will contain
data in case you receive the `HTTP_ON_BODY` event. **note: this is not a NULL terminated string**. 

_Example of doing a status update_
````c++

   // Include the necessary files:
   #include <twitter/Twitter.h>


   // the callback for the status update
   // --------------------------------------------------------------
   void twitter_status_cb(HTTPConnection* c, HTTPConnectionEvent event, 
                          const char* data, size_t len, void* user) 
   {
      if(event == HTTP_ON_STATUS) {
         RX_VERBOSE("HTTP status: %d", c->parser.status_code);
      }
      else if(event == HTTP_ON_BODY) {
         // parse the tweet.
         std::string str(data, data+len);
         Tweet tweet;
         tweet.parseJSON(str);
         tweet.print();
      }
   }


   // in your setup()       
   // --------------------------------------------------------------
   bool r =  twitter.setup("466622389-...",    /* the token */
                           "eH25IAxRIB...",    /* the token secret */
                           "e0vURm6xhS...",    /* the consumer key */
                           "R7HfL0vgy2...");   /* the consumer secret */
   if(!r) {
     RX_ERROR("Cannot setup the twitter obj");
     ::exit(EXIT_FAILURE);
   }

   // create a new status update + add an image to the post
   TwitterStatusesUpdate status("example status update with image :-)");
   status.addMedia("media.png", true); // true => get file from data folder
   twitter.apiStatusesUpdateWithMedia(status, twitter_status_cb, NULL);

   // in your update()
   // --------------------------------------------------------------
   twitter.update()

````